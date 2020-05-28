#pragma once

#include <cmath>
#include <hpkmediods/types/distance_matrix.hpp>
#include <hpkmediods/types/selected_set.hpp>
#include <matrix/matrix.hpp>
#include <type_traits>

namespace hpkmediods
{
template <typename T>
class Clusters
{
public:
    typedef SelectedSet::selected_t selected_t;
    typedef SelectedSet::unselected_t unselected_t;

    Clusters();

    Clusters(const Matrix<T>* const data, DistanceMatrix<T>* const distMat);

    bool operator<(const Clusters& lhs) const;

    bool operator>(const Clusters& lhs) const;

    void addCentroid(const int32_t dataIdx);

    void swapCentroid(const int32_t dataIdx, const int32_t centroidIdx);

    int32_t size() const;

    int32_t maxSize() const;

    const selected_t& selected() const;

    const unselected_t& unselected() const;

    int32_t numCandidates() const;

    const Matrix<T>* const getCentroids() const;

    const std::vector<int32_t>* const getClustering() const;

    const T getError() const;

    template <Parallelism Level>
    std::enable_if_t<Level == Parallelism::Serial || Level == Parallelism::MPI> calculateAssignments()
    {
        T cost = 0.0;

        for (int i = 0; i < static_cast<int32_t>(m_assignments.size()); ++i)
        {
            m_assignments[i] = p_distMat->getClosestCentroidIdx(i);
            cost += std::pow(p_distMat->distanceToCentroid(i, m_assignments[i]), 2);
        }

        m_error = cost;
    }

    template <Parallelism Level>
    std::enable_if_t<Level == Parallelism::OMP || Level == Parallelism::Hybrid> calculateAssignments()
    {
        T cost = 0.0;

#pragma omp parallel for schedule(static), reduction(+ : cost)
        for (int i = 0; i < static_cast<int32_t>(m_assignments.size()); ++i)
        {
            m_assignments[i] = p_distMat->getClosestCentroidIdx(i);
            cost += std::pow(p_distMat->distanceToCentroid(i, m_assignments[i]), 2);
        }

        m_error = cost;
    }

private:
    T m_error;
    const Matrix<T>* p_data;
    DistanceMatrix<T>* p_distMat;
    SelectedSet m_selectedSet;
    std::vector<int32_t> m_assignments;
    Matrix<T> m_centroids;
};
}  // namespace hpkmediods