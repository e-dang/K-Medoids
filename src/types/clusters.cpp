#include <hpkmediods/types/clusters.hpp>
#include <limits>

namespace hpkmediods
{
template <typename T>
Clusters<T>::Clusters() :
    m_error(std::numeric_limits<T>::max()),
    p_data(nullptr),
    p_distMat(nullptr),
    m_selectedSet(),
    m_assignments(),
    m_centroids()
{
}

template <typename T>
Clusters<T>::Clusters(const Matrix<T>* const data, DistanceMatrix<T>* const distMat) :
    m_error(std::numeric_limits<T>::max()),
    p_data(data),
    p_distMat(distMat),
    m_selectedSet(data->rows(), distMat->numCentroids()),
    m_assignments(data->rows()),
    m_centroids(distMat->numCentroids(), data->cols())
{
}

template <typename T>
bool Clusters<T>::operator<(const Clusters& lhs) const
{
    return m_error < lhs.m_error;
}

template <typename T>
bool Clusters<T>::operator>(const Clusters& lhs) const
{
    return m_error > lhs.m_error;
}

template <typename T>
void Clusters<T>::addCentroid(const int32_t dataIdx)
{
    m_centroids.append(p_data->crowBegin(dataIdx), p_data->crowEnd(dataIdx));
    m_selectedSet.select(dataIdx);
    p_distMat->updateDistancesToCentroid(dataIdx, size() - 1);
}

template <typename T>
void Clusters<T>::swapCentroid(const int32_t dataIdx, const int32_t centroidIdx)
{
    m_centroids.set(centroidIdx, p_data->crowBegin(dataIdx), p_data->crowEnd(dataIdx));
    m_selectedSet.replaceSelected(dataIdx, centroidIdx);
    p_distMat->updateDistancesToCentroid(dataIdx, centroidIdx);
}

template <typename T>
int32_t Clusters<T>::size() const
{
    return m_centroids.numRows();
}

template <typename T>
int32_t Clusters<T>::maxSize() const
{
    return m_centroids.rows();
}

template <typename T>
const typename Clusters<T>::selected_t& Clusters<T>::selected() const
{
    return m_selectedSet.selected();
}

template <typename T>
const typename Clusters<T>::unselected_t& Clusters<T>::unselected() const
{
    return m_selectedSet.unselected();
}

template <typename T>
int32_t Clusters<T>::numCandidates() const
{
    return m_selectedSet.unselectedSize();
}

template <typename T>
const Matrix<T>* const Clusters<T>::getCentroids() const
{
    return &m_centroids;
}

template <typename T>
const std::vector<int32_t>* const Clusters<T>::getClustering() const
{
    return &m_assignments;
}

template <typename T>
const T Clusters<T>::getError() const
{
    return m_error;
}

template class Clusters<float>;
template class Clusters<double>;
}  // namespace hpkmediods