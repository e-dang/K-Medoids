#pragma once

#include <hpkmediods/initializers/interface.hpp>
#include <hpkmediods/maximizers/interface.hpp>
#include <matrix/matrix.hpp>
#include <memory>
#include <set>
#include <vector>

namespace hpkmediods
{
template <typename T>
class ClusterData
{
public:
    ClusterData();

    ClusterData(const Matrix<T>* data, const int& numClusters);

    bool operator<(const ClusterData<T>& rhs) const;

    bool operator>(const ClusterData<T>& rhs) const;

    void initialize(const IInitializer<T>* const initializer);

    void maximize(const IMaximizer<T>* const maximizer);

    const Matrix<T>* const getCentroids() const;

    const std::vector<int32_t>* const getClustering() const;

    const T getError() const;

private:
    void initUnselected();

private:
    const Matrix<T>* p_data;
    Matrix<T> m_centroids;
    std::vector<int32_t> m_assignments;
    Matrix<T> m_dataDistMat;
    Matrix<T> m_centroidDistMat;
    std::vector<int32_t> m_selected;
    std::set<int32_t> m_unselected;
    T m_error;
};
}  // namespace hpkmediods