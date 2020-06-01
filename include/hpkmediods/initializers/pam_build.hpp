#pragma once

#include <algorithm>
#include <hpkmediods/initializers/interface.hpp>
#include <hpkmediods/utils/utils.hpp>
#include <limits>

namespace hpkmediods
{
constexpr char PAM_INIT[] = "pam_build";

template <typename T, Parallelism Level>
class PAMBuild : public IInitializer<T>
{
public:
    void initialize(const Matrix<T>* const data, Clusters<T>* const clusters,
                    DistanceMatrix<T>* const distMat) const override
    {
        initializeFirstCentroid(data, clusters, distMat);

        Matrix<T> dissimilarityMat(data->rows(), data->rows(), true, std::numeric_limits<T>::min());
        while (clusters->size() != clusters->maxSize())
        {
            updateDissimilarityMatrix(&dissimilarityMat, data, clusters, distMat);
            auto candidateIdx = getCandidateIdxForLargestGain(&dissimilarityMat);
            clusters->addCentroid(candidateIdx);
            dissimilarityMat.fill(std::numeric_limits<T>::min());
        }
    }

private:
    void initializeFirstCentroid(const Matrix<T>* const data, Clusters<T>* const clusters,
                                 DistanceMatrix<T>* const distMat) const
    {
        auto distanceSums = calculateDistanceSums(distMat);
        auto minIdx = std::distance(distanceSums.begin(), std::min_element(distanceSums.begin(), distanceSums.end()));
        clusters->addCentroid(minIdx);
    }

    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::Serial || _Level == Parallelism::MPI> updateDissimilarityMatrix(
      Matrix<T>* const dissimilarityMat, const Matrix<T>* const data, Clusters<T>* const clusters,
      const DistanceMatrix<T>* const distMat) const
    {
        for (const auto& candidateIdx : clusters->unselected())
        {
            updateDissimilarityMatrixImpl(candidateIdx, dissimilarityMat, data, clusters, distMat);
        }
    }

    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::OMP || _Level == Parallelism::Hybrid> updateDissimilarityMatrix(
      Matrix<T>* const dissimilarityMat, const Matrix<T>* const data, Clusters<T>* const clusters,
      const DistanceMatrix<T>* const distMat) const
    {
#pragma omp parallel for schedule(static)
        for (int32_t candidateIdx = 0; candidateIdx < static_cast<int32_t>(clusters->numCandidates()); ++candidateIdx)
        {
            updateDissimilarityMatrixImpl(candidateIdx, dissimilarityMat, data, clusters, distMat);
        }
    }

    void updateDissimilarityMatrixImpl(const int candidateIdx, Matrix<T>* const dissimilarityMat,
                                       const Matrix<T>* const data, Clusters<T>* const clusters,
                                       const DistanceMatrix<T>* const distMat) const
    {
        for (const auto& pointIdx : clusters->unselected())
        {
            if (candidateIdx != pointIdx)
            {
                auto distToClosestCentroid = distMat->distanceToClosestCentroid(pointIdx);
                auto candidatePointDist    = distMat->distanceToPoint(candidateIdx, pointIdx);
                dissimilarityMat->at(pointIdx, candidateIdx) =
                  std::max(distToClosestCentroid - candidatePointDist, 0.0);
            }
        }
    }

    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::Serial || _Level == Parallelism::MPI, std::vector<T>> calculateDistanceSums(
      const DistanceMatrix<T>* const distMat) const
    {
        std::vector<T> distanceSums(distMat->numPoints());
        for (int i = 0; i < distMat->numPoints(); ++i)
        {
            auto range      = distMat->getAllDistancesToPoints(i);
            distanceSums[i] = std::accumulate(range.first, range.second, 0.0);
        }

        return distanceSums;
    }

    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::OMP || _Level == Parallelism::Hybrid, std::vector<T>> calculateDistanceSums(
      const DistanceMatrix<T>* const distMat) const
    {
        std::vector<T> distanceSums(distMat->numPoints());

#pragma omp parallel for shared(distanceSums), schedule(static)
        for (int i = 0; i < distMat->numPoints(); ++i)
        {
            auto range      = distMat->getAllDistancesToPoints(i);
            distanceSums[i] = std::accumulate(range.first, range.second, 0.0);
        }

        return distanceSums;
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
};
}  // namespace hpkmediods