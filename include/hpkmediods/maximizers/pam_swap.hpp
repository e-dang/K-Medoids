#pragma once

#include <hpkmediods/maximizers/interface.hpp>
#include <hpkmediods/utils/clustering_updater.hpp>
#include <hpkmediods/utils/utils.hpp>

namespace hpkmediods
{
constexpr char PAM[] = "pam_swap";

template <typename T, Parallelism Level, class DistanceFunc>
class PAMSwap : public IMaximizer<T>
{
public:
    T maximize(const Matrix<T>* const data, Matrix<T>* const centroids, std::vector<int32_t>* const assignments,
               Matrix<T>* const dataDistMat, Matrix<T>* const centroidDistMat, std::vector<int32_t>* const unselected,
               std::vector<int32_t>* const selected) const override
    {
        while (true)
        {
            Matrix<T> dissimilarityMat(centroids->rows(), data->rows(), true, std::numeric_limits<T>::max());
            maximizeIter(&dissimilarityMat, data, centroids, dataDistMat, centroidDistMat, unselected, selected);

            auto minDissimilarity = *min_element(dissimilarityMat.cbegin(), dissimilarityMat.cend());
            if (minDissimilarity >= 0)
                break;

            auto coords = dissimilarityMat.find(minDissimilarity);
            centroids->set(coords.first, data->crowBegin(coords.second), data->crowEnd(coords.second));
            updateCentroidDistanceMatrix(coords.first, coords.second, centroidDistMat, dataDistMat);
            unselected->erase(std::find(unselected->cbegin(), unselected->cend(), coords.second));
            unselected->push_back(selected->at(coords.first));
            selected->at(coords.first) = coords.second;
        }

        m_updater.updateClusteringFromDistMat(centroidDistMat, assignments);
        return 0.0;
    }

private:
    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::Serial || _Level == Parallelism::MPI> maximizeIter(
      Matrix<T>* const dissimilarityMat, const Matrix<T>* const data, Matrix<T>* const centroids,
      Matrix<T>* const dataDistMat, Matrix<T>* const centroidDistMat, std::vector<int32_t>* const unselected,
      std::vector<int32_t>* const selected) const
    {
        for (int centroidIdx = 0; centroidIdx < centroids->rows(); ++centroidIdx)
        {
            std::vector<T> totals(data->rows());
            for (const auto& candidate : *unselected)
            {
                std::vector<T> contributionVec;
                contributionVec.reserve(unselected->size() - 1);
                for (const auto& point : *unselected)
                {
                    if (candidate != point)
                    {
                        auto centroidToPointDist =
                          m_distanceFunc(centroids->crowBegin(centroidIdx), centroids->crowEnd(centroidIdx),
                                         data->crowBegin(point), data->crowEnd(point));

                        auto pointToClosestCentroidDist =
                          *min_element(centroidDistMat->crowBegin(point), centroidDistMat->crowEnd(point));

                        auto pointToCandidateDist = m_distanceFunc(data->crowBegin(candidate), data->crowEnd(candidate),
                                                                   data->crowBegin(point), data->crowEnd(point));

                        if (centroidToPointDist > pointToClosestCentroidDist)
                            contributionVec.emplace_back(
                              std::min(pointToCandidateDist - pointToClosestCentroidDist, 0.0));
                        else if (centroidToPointDist == pointToClosestCentroidDist)
                        {
                            auto pointToSecondClosestCentroidDist =
                              getSecondLowest(centroidDistMat->crowBegin(point), centroidDistMat->crowEnd(point));
                            contributionVec.emplace_back(
                              std::min(pointToCandidateDist, pointToSecondClosestCentroidDist) -
                              pointToClosestCentroidDist);
                        }
                    }
                }
                totals[candidate] = std::accumulate(contributionVec.cbegin(), contributionVec.cend(), 0.0);
            }

            dissimilarityMat->set(centroidIdx, totals);
        }
    }

    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::OMP || _Level == Parallelism::Hybrid> maximizeIter(
      Matrix<T>* const dissimilarityMat, const Matrix<T>* const data, Matrix<T>* const centroids,
      Matrix<T>* const dataDistMat, Matrix<T>* const centroidDistMat, std::vector<int32_t>* const unselected,
      std::vector<int32_t>* const selected) const
    {
#pragma omp parallel for shared(dissimilarityMat, data, centroids, dataDistMat, centroidDistMat, unselected, selected)
        for (int centroidIdx = 0; centroidIdx < centroids->rows(); ++centroidIdx)
        {
            std::vector<T> totals(data->rows());
            for (const auto& candidate : *unselected)
            {
                std::vector<T> contributionVec;
                contributionVec.reserve(unselected->size() - 1);
                for (const auto& point : *unselected)
                {
                    if (candidate != point)
                    {
                        auto centroidToPointDist =
                          m_distanceFunc(centroids->crowBegin(centroidIdx), centroids->crowEnd(centroidIdx),
                                         data->crowBegin(point), data->crowEnd(point));

                        auto pointToClosestCentroidDist =
                          *min_element(centroidDistMat->crowBegin(point), centroidDistMat->crowEnd(point));

                        auto pointToCandidateDist = m_distanceFunc(data->crowBegin(candidate), data->crowEnd(candidate),
                                                                   data->crowBegin(point), data->crowEnd(point));

                        if (centroidToPointDist > pointToClosestCentroidDist)
                            contributionVec.emplace_back(
                              std::min(pointToCandidateDist - pointToClosestCentroidDist, 0.0));
                        else if (centroidToPointDist == pointToClosestCentroidDist)
                        {
                            auto pointToSecondClosestCentroidDist =
                              getSecondLowest(centroidDistMat->crowBegin(point), centroidDistMat->crowEnd(point));
                            contributionVec.emplace_back(
                              std::min(pointToCandidateDist, pointToSecondClosestCentroidDist) -
                              pointToClosestCentroidDist);
                        }
                    }
                }
                totals[candidate] = std::accumulate(contributionVec.cbegin(), contributionVec.cend(), 0.0);
            }

            dissimilarityMat->set(centroidIdx, totals);
        }
    }

    void updateCentroidDistanceMatrix(const int32_t centroidIdx, const int32_t dataIdx,
                                      Matrix<T>* const centroidDistMat, const Matrix<T>* const dataDistMat) const
    {
        for (int i = 0; i < dataDistMat->rows(); ++i)
        {
            centroidDistMat->at(i, centroidIdx) = dataDistMat->at(i, dataIdx);
        }
    }

private:
    ClusteringUpdater<T, Level> m_updater;
    DistanceFunc m_distanceFunc;
};
}  // namespace hpkmediods