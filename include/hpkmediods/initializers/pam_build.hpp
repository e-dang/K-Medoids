#pragma once

#include <algorithm>
#include <hpkmediods/distance_calculator.hpp>
#include <hpkmediods/initializers/interface.hpp>
#include <hpkmediods/utils/clustering_updater.hpp>
#include <hpkmediods/utils/utils.hpp>
#include <limits>
#include <matrix/matrix.hpp>

namespace hpkmediods
{
constexpr char PAM_INIT[] = "pam_build";

template <typename T, Parallelism Level, class DistanceFunc>
class PAMBuild : public IInitializer<T>
{
public:
    void initialize(const Matrix<T>* const data, Matrix<T>* const centroids, std::vector<int32_t>* const assignments,
                    Matrix<T>* const dataDistMat, Matrix<T>* const centroidDistMat,
                    SelectedSet* const selectedSet) const override
    {
        m_distanceCalc.calculateDistanceMatrix(data, data, dataDistMat);
        initializeFirstCentroid(data, dataDistMat, centroids, centroidDistMat, selectedSet);

        Matrix<T> dissimilarityMat(data->rows(), data->rows(), true, std::numeric_limits<T>::min());
        while (centroids->numRows() != centroids->rows())
        {
            updateDissimilarityMatrix(&dissimilarityMat, data, centroidDistMat, selectedSet);
            auto candidateIdx = getCandidateIdxForLargestGain(&dissimilarityMat);

            std::copy(dataDistMat->ccolBegin(candidateIdx), dataDistMat->ccolEnd(candidateIdx),
                      centroidDistMat->colBegin(centroids->numRows()));
            centroids->append(data->crowBegin(candidateIdx), data->crowEnd(candidateIdx));
            selectedSet->select(candidateIdx);

            dissimilarityMat.fill(std::numeric_limits<T>::min());
        }

        m_updater.updateClusteringFromDistMat(centroidDistMat, assignments);
    }

private:
    void initializeFirstCentroid(const Matrix<T>* const data, const Matrix<T>* const dataDistMat,
                                 Matrix<T>* const centroids, Matrix<T>* const centroidDistMat,
                                 SelectedSet* const selectedSet) const
    {
        auto distanceSums = m_distanceCalc.calculateDistanceSums(dataDistMat);
        auto minIdx = std::distance(distanceSums.begin(), std::min_element(distanceSums.begin(), distanceSums.end()));
        centroids->append(data->crowBegin(minIdx), data->crowEnd(minIdx));
        selectedSet->select(minIdx);
        std::copy(dataDistMat->ccolBegin(minIdx), dataDistMat->ccolEnd(minIdx), centroidDistMat->colBegin(0));
    }

    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::Serial || _Level == Parallelism::MPI> updateDissimilarityMatrix(
      Matrix<T>* const dissimilarityMat, const Matrix<T>* const data, const Matrix<T>* const centroidDistMat,
      SelectedSet* const selectedSet) const
    {
        for (const auto& candidateIdx : selectedSet->unselected())
        {
            updateDissimilarityMatrixImpl(candidateIdx, dissimilarityMat, data, centroidDistMat, selectedSet);
        }
    }

    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::OMP || _Level == Parallelism::Hybrid> updateDissimilarityMatrix(
      Matrix<T>* const dissimilarityMat, const Matrix<T>* const data, const Matrix<T>* const centroidDistMat,
      SelectedSet* const selectedSet) const
    {
#pragma omp parallel for shared(dissimilarityMat, data, centroidDistMat, selectedSet), schedule(static)
        for (int32_t candidateIdx = 0; candidateIdx < static_cast<int32_t>(selectedSet->unselectedSize());
             ++candidateIdx)
        {
            updateDissimilarityMatrixImpl(candidateIdx, dissimilarityMat, data, centroidDistMat, selectedSet);
        }
    }

    void updateDissimilarityMatrixImpl(const int candidateIdx, Matrix<T>* const dissimilarityMat,
                                       const Matrix<T>* const data, const Matrix<T>* const centroidDistMat,
                                       SelectedSet* const selectedSet) const
    {
        for (const auto& pointIdx : selectedSet->unselected())
        {
            if (candidateIdx != pointIdx)
            {
                auto pointToClosestObjDist =
                  *min_element(centroidDistMat->crowBegin(pointIdx), centroidDistMat->crowEnd(pointIdx));
                auto candidatePointDist = m_distanceFunc(data->crowBegin(candidateIdx), data->crowEnd(candidateIdx),
                                                         data->crowBegin(pointIdx), data->crowEnd(pointIdx));
                dissimilarityMat->at(pointIdx, candidateIdx) =
                  std::max(pointToClosestObjDist - candidatePointDist, 0.0);
            }
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