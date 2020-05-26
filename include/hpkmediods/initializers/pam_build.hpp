#pragma once

#include <algorithm>
#include <hpkmediods/distance_calculator.hpp>
#include <hpkmediods/initializers/interface.hpp>
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
    T initialize(const Matrix<T>* const data, Matrix<T>& centroids, std::vector<int32_t>& assignments,
                 std::vector<T>& sqDistances) const override
    {
        Matrix<T> objDistMat(data->rows(), centroids.rows(), true, std::numeric_limits<T>::max());
        auto unselected = createUnselectedSet(data->rows());
        auto distMat    = m_distanceCalc.calculateDistanceMatrix(data, data);
        initializeFirstCentroid(data, &distMat, &centroids, &objDistMat);

        while (centroids.numRows() != centroids.rows())
        {
            auto dissimilarityMat = calculateDissimilarityMatrix(data, &objDistMat, unselected);
            auto candidateIdx     = getCandidateIdxForLargestGain(&dissimilarityMat);

            updateObjDistanceMatrix(centroids.numRows(), candidateIdx, &objDistMat, &distMat);
            centroids.append(data->crowBegin(candidateIdx), data->crowEnd(candidateIdx));
            unselected.erase(candidateIdx);
        }

        findAndUpdateClosestCentroidsFromDistMat(&objDistMat, assignments, sqDistances);
        return std::accumulate(sqDistances.begin(), sqDistances.end(), 0.0);
    }

private:
    void initializeFirstCentroid(const Matrix<T>* const data, const Matrix<T>* const distMat,
                                 Matrix<T>* const centroids, Matrix<T>* const objDistMat) const
    {
        auto distanceSums = m_distanceCalc.calculateDistanceSums(distMat);
        auto minIdx = std::distance(distanceSums.begin(), std::min_element(distanceSums.begin(), distanceSums.end()));
        centroids->append(data->crowBegin(minIdx), data->crowEnd(minIdx));
        updateObjDistanceMatrix(0, minIdx, objDistMat, distMat);
    }

    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::Serial || _Level == Parallelism::MPI, Matrix<T>>
      calculateDissimilarityMatrix(const Matrix<T>* const data, const Matrix<T>* const objDistMat,
                                   std::set<int32_t>& unselected) const
    {
        Matrix<T> dissimilarityMat(data->rows(), data->rows(), true, std::numeric_limits<T>::min());
        for (const auto& candidateIdx : unselected)
        {
            for (const auto& pointIdx : unselected)
            {
                if (candidateIdx != pointIdx)
                {
                    auto pointToClosestObjDist =
                      min_element(objDistMat->crowBegin(pointIdx), objDistMat->crowEnd(pointIdx));
                    auto candidatePointDist = m_distanceFunc(data->crowBegin(candidateIdx), data->crowEnd(candidateIdx),
                                                             data->crowBegin(pointIdx), data->crowEnd(pointIdx));
                    dissimilarityMat.at(pointIdx, candidateIdx) =
                      std::max(*pointToClosestObjDist - candidatePointDist, 0.0);
                }
            }
        }

        return dissimilarityMat;
    }

    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::OMP || _Level == Parallelism::Hybrid, Matrix<T>>
      calculateDissimilarityMatrix(const Matrix<T>* const data, const Matrix<T>* const objDistMat,
                                   std::set<int32_t>& unselected) const
    {
        Matrix<T> dissimilarityMat(data->rows(), data->rows(), true, std::numeric_limits<T>::min());

#pragma omp parallel for shared(dissimilarityMat, data, objDistMat, unselected), schedule(static)
        for (auto candidateIter = unselected.cbegin(); candidateIter != unselected.cend(); ++candidateIter)
        {
            for (const auto& pointIdx : unselected)
            {
                if (*candidateIter != pointIdx)
                {
                    auto pointToClosestObjDist =
                      min_element(objDistMat->crowBegin(pointIdx), objDistMat->crowEnd(pointIdx));
                    auto candidatePointDist =
                      m_distanceFunc(data->crowBegin(*candidateIter), data->crowEnd(*candidateIter),
                                     data->crowBegin(pointIdx), data->crowEnd(pointIdx));
                    dissimilarityMat.at(pointIdx, *candidateIter) =
                      std::max(*pointToClosestObjDist - candidatePointDist, 0.0);
                }
            }
        }

        return dissimilarityMat;
    }

    std::set<int32_t> createUnselectedSet(const int32_t numElements) const
    {
        std::set<int32_t> unselected;
        auto iter = unselected.begin();
        for (int i = 0; i < numElements; ++i)
        {
            iter = unselected.insert(iter, i);
        }

        return unselected;
    }

    void updateObjDistanceMatrix(const int32_t centroidIdx, const int32_t dataIdx,
                                 Matrix<T>* const dataToCentroidDistMat, const Matrix<T>* const dataDistMat) const
    {
        for (int i = 0; i < dataDistMat->rows(); ++i)
        {
            dataToCentroidDistMat->at(i, centroidIdx) = dataDistMat->at(i, dataIdx);
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

    std::vector<int32_t> findAndUpdateClosestCentroidsFromDistMat(const Matrix<T>* const objDistMat,
                                                                  std::vector<int32_t>& assignments,
                                                                  std::vector<T>& sqDistances) const
    {
        std::vector<int32_t> vec;
        for (int i = 0; i < static_cast<int32_t>(assignments.size()); ++i)
        {
            auto distIter = min_element(objDistMat->crowBegin(i), objDistMat->crowEnd(i));
            auto sqDist   = std::pow(*distIter, 2);
            if (sqDist < sqDistances[i])
            {
                sqDistances[i] = sqDist;
                assignments[i] = std::distance(objDistMat->crowBegin(i), distIter);
            }
        }

        return vec;
    }

private:
    DistanceFunc m_distanceFunc;
    DistanceCalculator<T, Level, DistanceFunc> m_distanceCalc;
};
}  // namespace hpkmediods