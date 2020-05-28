#include <hpkmediods/types/cluster_data.hpp>
#include <limits>

namespace hpkmediods
{
template <typename T>
ClusterData<T>::ClusterData() :
    p_data(nullptr),
    m_centroids(),
    m_assignments(),
    m_dataDistMat(),
    m_centroidDistMat(),
    m_selectedSet(),
    m_error(std::numeric_limits<T>::max())
{
}

template <typename T>
ClusterData<T>::ClusterData(const Matrix<T>* const data, const int& numClusters) :
    p_data(data),
    m_centroids(numClusters, data->cols()),
    m_assignments(data->rows(), -1),
    m_dataDistMat(data->rows(), data->rows(), true, std::numeric_limits<T>::max()),
    m_centroidDistMat(data->rows(), numClusters, true, std::numeric_limits<T>::max()),
    m_selectedSet(data->rows(), numClusters),
    m_error(std::numeric_limits<T>::max())
{
}

template <typename T>
bool ClusterData<T>::operator<(const ClusterData<T>& rhs) const
{
    return m_error < rhs.m_error;
}

template <typename T>
bool ClusterData<T>::operator>(const ClusterData<T>& rhs) const
{
    return m_error > rhs.m_error;
}

template <typename T>
void ClusterData<T>::initialize(const IInitializer<T>* const initializer)
{
    initializer->initialize(p_data, &m_centroids, &m_assignments, &m_dataDistMat, &m_centroidDistMat, &m_selectedSet);
}

template <typename T>
void ClusterData<T>::maximize(const IMaximizer<T>* const maximizer)
{
    m_error =
      maximizer->maximize(p_data, &m_centroids, &m_assignments, &m_dataDistMat, &m_centroidDistMat, &m_selectedSet);
}

template <typename T>
const Matrix<T>* const ClusterData<T>::getCentroids() const
{
    return &m_centroids;
}

template <typename T>
const std::vector<int32_t>* const ClusterData<T>::getClustering() const
{
    return &m_assignments;
}

template <typename T>
const T ClusterData<T>::getError() const
{
    return m_error;
}

template class ClusterData<double>;
template class ClusterData<float>;
}  // namespace hpkmediods