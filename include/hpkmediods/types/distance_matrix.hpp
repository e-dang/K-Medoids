#pragma once

#include <hpkmediods/types/parallelism.hpp>
#include <hpkmediods/utils/distance_calculator.hpp>
#include <matrix/matrix.hpp>
#include <utility>

namespace hpkmediods
{
template <typename T>
class Clusters;

template <typename T>
class DistanceMatrix
{
public:
    typedef typename Matrix<T>::const_row_iterator const_iterator;

    DistanceMatrix();

    template <Parallelism Level, class DistanceFunc>
    static DistanceMatrix<T> create(const Matrix<T>* const data, const int32_t numClusters)
    {
        DistanceCalculator<T, Level, DistanceFunc> distanceCalc;
        return DistanceMatrix<T>(distanceCalc.calculateDistanceMatrix(data, data), numClusters);
    }

    T distanceToClosestCentroid(const int32_t dataIdx) const;

    T distanceToCentroid(const int32_t dataIdx, const int32_t centroidIdx) const;

    T distanceToPoint(const int32_t firstIdx, const int32_t secondIdx) const;

    T calcTotalDistanceToAllPoints(const int32_t dataIdx) const;

    std::pair<const_iterator, const_iterator> getAllDistancesToPoints(const int32_t dataIdx) const;

    std::pair<const_iterator, const_iterator> getAllDistancesToCentroids(const int32_t dataIdx) const;

    int32_t getClosestCentroidIdx(const int32_t dataIdx) const;

    void updateDistancesToCentroid(const int32_t dataIdx, const int32_t centroidIdx);

    int32_t numPoints() const;

    int32_t numCentroids() const;

private:
    DistanceMatrix(Matrix<T>&& dataDistMat, const int32_t numClusters);

private:
    Matrix<T> m_dataDistMat;
    Matrix<T> m_centroidDistMat;
};
}  // namespace hpkmediods