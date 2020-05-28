#pragma once

#include <hpkmediods/maximizers/interface.hpp>
#include <hpkmediods/utils/utils.hpp>

namespace hpkmediods
{
constexpr char PAM[] = "pam_swap";

template <typename T, Parallelism Level, class DistanceFunc>
class PAMSwap : public IMaximizer<T>
{
public:
    void maximize(const Matrix<T>* const data, Clusters<T>* const clusters,
                  DistanceMatrix<T>* const distMat) const override
    {
        while (true)
        {
            Matrix<T> dissimilarityMat(clusters->size(), data->rows(), true, std::numeric_limits<T>::max());
            maximizeIter(&dissimilarityMat, data, clusters, distMat);

            auto minDissimilarity = *min_element(dissimilarityMat.cbegin(), dissimilarityMat.cend());
            if (minDissimilarity >= 0)
                break;

            auto coords = dissimilarityMat.find(minDissimilarity);
            clusters->swapCentroid(coords.second, coords.first);
        }

        clusters->template calculateAssignments<Level>();
    }

private:
    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::Serial || _Level == Parallelism::MPI> maximizeIter(
      Matrix<T>* const dissimilarityMat, const Matrix<T>* const data, Clusters<T>* const clusters,
      DistanceMatrix<T>* const distMat) const
    {
        for (int centroidIdx = 0; centroidIdx < clusters->size(); ++centroidIdx)
        {
            maximizeIterImpl(centroidIdx, dissimilarityMat, data, clusters, distMat);
        }
    }

    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::OMP || _Level == Parallelism::Hybrid> maximizeIter(
      Matrix<T>* const dissimilarityMat, const Matrix<T>* const data, Clusters<T>* const clusters,
      DistanceMatrix<T>* const distMat) const
    {
#pragma omp parallel for shared(dissimilarityMat, data, clusters, distMat)
        for (int centroidIdx = 0; centroidIdx < clusters->size(); ++centroidIdx)
        {
            maximizeIterImpl(centroidIdx, dissimilarityMat, data, clusters, distMat);
        }
    }

    void maximizeIterImpl(const int centroidIdx, Matrix<T>* const dissimilarityMat, const Matrix<T>* const data,
                          Clusters<T>* const clusters, DistanceMatrix<T>* const distMat) const
    {
        std::vector<T> totals(data->rows());
        for (const auto& candidate : clusters->unselected())
        {
            std::vector<T> contributionVec;
            contributionVec.reserve(clusters->numCandidates() - 1);
            for (const auto& point : clusters->unselected())
            {
                if (candidate != point)
                {
                    auto centroidToPointDist        = distMat->distanceToCentroid(point, centroidIdx);
                    auto pointToClosestCentroidDist = distMat->distanceToClosestCentroid(point);
                    auto pointToCandidateDist       = distMat->distanceToPoint(candidate, point);

                    if (centroidToPointDist > pointToClosestCentroidDist)
                        contributionVec.emplace_back(std::min(pointToCandidateDist - pointToClosestCentroidDist, 0.0));
                    else if (centroidToPointDist == pointToClosestCentroidDist)
                    {
                        auto range                            = distMat->getAllDistancesToCentroids(point);
                        auto pointToSecondClosestCentroidDist = getSecondLowest(range.first, range.second);
                        contributionVec.emplace_back(std::min(pointToCandidateDist, pointToSecondClosestCentroidDist) -
                                                     pointToClosestCentroidDist);
                    }
                }
            }
            totals[candidate] = std::accumulate(contributionVec.cbegin(), contributionVec.cend(), 0.0);
        }

        dissimilarityMat->set(centroidIdx, totals);
    }
};
}  // namespace hpkmediods