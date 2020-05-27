#pragma once

#include <algorithm>
#include <hpkmediods/distance_calculator.hpp>
#include <hpkmediods/initializers/interface.hpp>
#include <hpkmediods/utils/clustering_updater.hpp>
#include <hpkmediods/utils/utils.hpp>
#include <limits>
#include <matrix/matrix.hpp>
#include <set>
#include <vector>

namespace hpkmediods
{
constexpr char PAM_INIT[] = "pam_build";

template <typename T, Parallelism Level, class DistanceFunc>
class PAMBuild : public IInitializer<T>
{
public:
    void initialize(const Matrix<T>* const data, Matrix<T>* const centroids, std::vector<int32_t>* const assignments,
                    Matrix<T>* const dataDistMat, Matrix<T>* const centroidDistMat, std::set<int32_t>* const unselected,
                    std::vector<int32_t>* const selected) const override
    {
        m_distanceCalc.calculateDistanceMatrix(data, data, dataDistMat);
        initializeFirstCentroid(data, dataDistMat, centroids, centroidDistMat, unselected, selected);

        Matrix<T> dissimilarityMat(data->rows(), data->rows(), true, std::numeric_limits<T>::min());
        while (centroids->numRows() != centroids->rows())
        {
            updateDissimilarityMatrix(&dissimilarityMat, data, centroidDistMat, unselected);
            auto candidateIdx = getCandidateIdxForLargestGain(&dissimilarityMat);

            updateCentroidDistanceMatrix(centroids->numRows(), candidateIdx, centroidDistMat, dataDistMat);
            centroids->append(data->crowBegin(candidateIdx), data->crowEnd(candidateIdx));
            unselected->erase(candidateIdx);
            selected->push_back(candidateIdx);

            dissimilarityMat.fill(std::numeric_limits<T>::min());
        }

        m_updater.updateClusteringFromDistMat(centroidDistMat, assignments);
    }

private:
    void initializeFirstCentroid(const Matrix<T>* const data, const Matrix<T>* const distMat,
                                 Matrix<T>* const centroids, Matrix<T>* const centroidDistMat,
                                 std::set<int32_t>* unselected, std::vector<int32_t>* selected) const
    {
        auto distanceSums = m_distanceCalc.calculateDistanceSums(distMat);
        auto minIdx = std::distance(distanceSums.begin(), std::min_element(distanceSums.begin(), distanceSums.end()));
        centroids->append(data->crowBegin(minIdx), data->crowEnd(minIdx));
        unselected->erase(minIdx);
        selected->push_back(minIdx);
        updateCentroidDistanceMatrix(0, minIdx, centroidDistMat, distMat);
    }

    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::Serial || _Level == Parallelism::MPI> updateDissimilarityMatrix(
      Matrix<T>* const dissimilarityMat, const Matrix<T>* const data, const Matrix<T>* const centroidDistMat,
      std::set<int32_t>* const unselected) const
    {
        for (const auto& candidateIdx : *unselected)
        {
            for (const auto& pointIdx : *unselected)
            {
                if (candidateIdx != pointIdx)
                {
                    auto pointToClosestObjDist =
                      min_element(centroidDistMat->crowBegin(pointIdx), centroidDistMat->crowEnd(pointIdx));
                    auto candidatePointDist = m_distanceFunc(data->crowBegin(candidateIdx), data->crowEnd(candidateIdx),
                                                             data->crowBegin(pointIdx), data->crowEnd(pointIdx));
                    dissimilarityMat->at(pointIdx, candidateIdx) =
                      std::max(*pointToClosestObjDist - candidatePointDist, 0.0);
                }
            }
        }
    }

    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::OMP || _Level == Parallelism::Hybrid> updateDissimilarityMatrix(
      Matrix<T>* const dissimilarityMat, const Matrix<T>* const data, const Matrix<T>* const centroidDistMat,
      std::set<int32_t>* const unselected) const
    {
#pragma omp parallel for shared(dissimilarityMat, data, centroidDistMat, unselected), schedule(static)
        for (auto candidateIter = unselected->cbegin(); candidateIter != unselected->cend(); ++candidateIter)
        {
            for (const auto& pointIdx : *unselected)
            {
                if (*candidateIter != pointIdx)
                {
                    auto pointToClosestObjDist =
                      min_element(centroidDistMat->crowBegin(pointIdx), centroidDistMat->crowEnd(pointIdx));
                    auto candidatePointDist =
                      m_distanceFunc(data->crowBegin(*candidateIter), data->crowEnd(*candidateIter),
                                     data->crowBegin(pointIdx), data->crowEnd(pointIdx));
                    dissimilarityMat->at(pointIdx, *candidateIter) =
                      std::max(*pointToClosestObjDist - candidatePointDist, 0.0);
                }
            }
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

    int32_t getCandidateIdxForLargestGain(const Matrix<T>* const dissimilarityMat) const
    {
        int32_t maxIdx = -1;
        auto maxGain   = std::numeric_limits<T>::min();
        for (int i = 0; i < dissimilarityMat->rows(); ++i)
        {
            auto gain = std::accumulate(dissimilarityMat->crowBegin(i), dissimilarityMat->crowEnd(i), 0.0);
            if (gain > maxGain)
            {
                maxIdx  = i;
                maxGain = gain;
            }
        }

        return maxIdx;
    }

private:
    DistanceFunc m_distanceFunc;
    ClusteringUpdater<T, Level> m_updater;
    DistanceCalculator<T, Level, DistanceFunc> m_distanceCalc;
};
}  // namespace hpkmediods