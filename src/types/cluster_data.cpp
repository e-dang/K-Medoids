#include <hpkmediods/types/cluster_data.hpp>

namespace hpkmediods
{
template <typename T>
ClusterData<T>::ClusterData() : p_data(nullptr), m_centroids(), m_assignments(), m_distances(), m_error(-1.0)
{
}

template <typename T>
ClusterData<T>::ClusterData(const Matrix<T>* const data, const int& numClusters) :
    p_data(data),
    m_centroids(numClusters, data->cols()),
    m_assignments(data->rows(), -1),
    m_distances(data->rows(), -1.0),
    m_error(-1.0)
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
    initializer->initialize(p_data, &m_centroids, &m_assignments, &m_distances);
}

template <typename T>
void ClusterData<T>::maximize(const IMaximizer<T>* const maximizer)
{
    m_error = maximizer->maximize(p_data, &m_centroids, &m_assignments, &m_distances);
}

template class ClusterData<double>;
template class ClusterData<float>;
}  // namespace hpkmediods