#include <hpkmedoids/types/distance_matrix.hpp>
#include <hpkmedoids/utils/utils.hpp>
#include <limits>
#include <numeric>

namespace hpkmedoids
{
template <typename T>
DistanceMatrix<T>::DistanceMatrix()
{
}

template <typename T>
DistanceMatrix<T>::DistanceMatrix(Matrix<T>&& dataDistMat, const int32_t numClusters) :
    m_dataDistMat(std::move(dataDistMat)),
    m_centroidDistMat(m_dataDistMat.rows(), numClusters, true, std::numeric_limits<T>::max())
{
}

template <typename T>
T DistanceMatrix<T>::distanceToClosestCentroid(const int32_t dataIdx) const
{
    return *min_element(m_centroidDistMat.crowBegin(dataIdx), m_centroidDistMat.crowEnd(dataIdx));
}

template <typename T>
T DistanceMatrix<T>::distanceToCentroid(const int32_t dataIdx, const int32_t centroidIdx) const
{
    return m_centroidDistMat.at(dataIdx, centroidIdx);
}

template <typename T>
T DistanceMatrix<T>::distanceToPoint(const int32_t firstIdx, const int32_t secondIdx) const
{
    return m_dataDistMat.at(firstIdx, secondIdx);
}

template <typename T>
T DistanceMatrix<T>::calcTotalDistanceToAllPoints(const int32_t dataIdx) const
{
    return std::accumulate(m_dataDistMat.crowBegin(dataIdx), m_dataDistMat.crowEnd(dataIdx), 0.0);
}

template <typename T>
std::pair<typename DistanceMatrix<T>::const_iterator, typename DistanceMatrix<T>::const_iterator>
  DistanceMatrix<T>::getAllDistancesToPoints(const int32_t dataIdx) const
{
    return std::make_pair<DistanceMatrix<T>::const_iterator, DistanceMatrix<T>::const_iterator>(
      m_dataDistMat.crowBegin(dataIdx), m_dataDistMat.crowEnd(dataIdx));
}

template <typename T>
std::pair<typename DistanceMatrix<T>::const_iterator, typename DistanceMatrix<T>::const_iterator>
  DistanceMatrix<T>::getAllDistancesToCentroids(const int32_t dataIdx) const
{
    return std::make_pair<DistanceMatrix<T>::const_iterator, DistanceMatrix<T>::const_iterator>(
      m_centroidDistMat.crowBegin(dataIdx), m_centroidDistMat.crowEnd(dataIdx));
}

template <typename T>
int32_t DistanceMatrix<T>::getClosestCentroidIdx(const int32_t dataIdx) const
{
    return std::distance(m_centroidDistMat.crowBegin(dataIdx),
                         min_element(m_centroidDistMat.crowBegin(dataIdx), m_centroidDistMat.crowEnd(dataIdx)));
}

template <typename T>
void DistanceMatrix<T>::updateDistancesToCentroid(const int32_t dataIdx, const int32_t centroidIdx)
{
    std::copy(m_dataDistMat.ccolBegin(dataIdx), m_dataDistMat.ccolEnd(dataIdx),
              m_centroidDistMat.colBegin(centroidIdx));
}

template <typename T>
int32_t DistanceMatrix<T>::numPoints() const
{
    return m_dataDistMat.rows();
}

template <typename T>
int32_t DistanceMatrix<T>::numCentroids() const
{
    return m_centroidDistMat.cols();
}

template class DistanceMatrix<float>;
template class DistanceMatrix<double>;
}  // namespace hpkmedoids