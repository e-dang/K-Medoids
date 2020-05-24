#pragma once

#include <hpkmediods/types/parallelism.hpp>
#include <limits>
#include <matrix/matrix.hpp>
#include <type_traits>

namespace hpkmediods
{
template <typename T>
struct ClosestCentroid
{
    int32_t centroidIdx;
    T sqDistance;
};

template <typename T, class DistanceFunc>
class AbstractCentroidFinder
{
public:
    virtual void findAndUpdateClosestCentroids(const Matrix<T>* const data, const Matrix<T>& centroids,
                                               std::vector<int32_t>& assignments,
                                               std::vector<T>& sqDistances) const = 0;

    template <typename Iter>
    ClosestCentroid<T> findClosestCentroid(Iter dataBeginIter, Iter dataEndIter, const Matrix<T>& centroids) const
    {
        T minDistance      = std::numeric_limits<T>::max();
        int32_t clusterIdx = -1;

        for (int32_t i = 0; i < centroids.numRows(); ++i)
        {
            auto tempDistance =
              m_distanceFunc(dataBeginIter, dataEndIter, centroids.crowBegin(i), centroids.crowEnd(i));

            if (tempDistance < minDistance)
            {
                minDistance = tempDistance;
                clusterIdx  = i;
            }
        }

        return ClosestCentroid<T>{ clusterIdx, static_cast<T>(std::pow(minDistance, 2)) };
    }

protected:
    DistanceFunc m_distanceFunc;
};

template <typename T, Parallelism Level, class DistanceFunc>
class CentroidFinder : public AbstractCentroidFinder<T, DistanceFunc>
{
public:
    void findAndUpdateClosestCentroids(const Matrix<T>* const data, const Matrix<T>& centroids,
                                       std::vector<int32_t>& assignments, std::vector<T>& sqDistances) const override
    {
        findAndUpdateClosestCentroidsImpl(data, centroids, assignments, sqDistances);
    }

private:
    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::Serial || _Level == Parallelism::MPI> findAndUpdateClosestCentroidsImpl(
      const Matrix<T>* const data, const Matrix<T>& centroids, std::vector<int32_t>& assignments,
      std::vector<T>& sqDistances) const
    {
        for (int32_t i = 0; i < data->rows(); ++i)
        {
            auto closestCentroid = this->findClosestCentroid(data->crowBegin(i), data->crowEnd(i), centroids);
            if (closestCentroid.sqDistance < sqDistances[i])
            {
                assignments[i] = closestCentroid.centroidIdx;
                sqDistances[i] = closestCentroid.sqDistance;
            }
        }
    }

    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::OMP || _Level == Parallelism::Hybrid> findAndUpdateClosestCentroidsImpl(
      const Matrix<T>* const data, const Matrix<T>& centroids, std::vector<int32_t>& assignments,
      std::vector<T>& sqDistances) const
    {
#pragma omp parallel for shared(data, centroids, assignments, sqDistances), schedule(static)
        for (int32_t i = 0; i < data->rows(); ++i)
        {
            auto closestCentroid = this->findClosestCentroid(data->crowBegin(i), data->crowEnd(i), centroids);
            if (closestCentroid.sqDistance < sqDistances[i])
            {
                assignments[i] = closestCentroid.centroidIdx;
                sqDistances[i] = closestCentroid.sqDistance;
            }
        }
    }
};
}  // namespace hpkmediods