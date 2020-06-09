#pragma once

#include <cmath>
#include <hpkmedoids/types/distance_matrix.hpp>
#include <hpkmedoids/types/selected_set.hpp>
#include <iostream>
#include <matrix/matrix.hpp>
#include <type_traits>
namespace hpkmedoids
{
template <typename T>
class Clusters
{
public:
    typedef SelectedSet::selected_t selected_t;
    typedef SelectedSet::unselected_t unselected_t;

    Clusters();

    Clusters(const Matrix<T>* const data, const Matrix<T>* const centroids);

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
    std::enable_if_t<Level == Parallelism::Serial || Level == Parallelism::MPI> calculateAssignmentsFromDistMat()
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
    std::enable_if_t<Level == Parallelism::OMP || Level == Parallelism::Hybrid> calculateAssignmentsFromDistMat()
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

    template <Parallelism Level, class DistanceFunc>
    std::enable_if_t<Level == Parallelism::Serial || Level == Parallelism::MPI> calculateAssignmentsFromCentroids(
      DistanceFunc& distanceFunc)
    {
        T cost = 0.0;

        for (int32_t i = 0; i < p_data->rows(); ++i)
        {
            auto closestCentroid = findClosestCentroid(p_data->crowBegin(i), p_data->crowEnd(i), distanceFunc);
            m_assignments[i]     = closestCentroid.idx;
            cost += std::pow(closestCentroid.distance, 2);
        }

        m_error = cost;
    }

    template <Parallelism Level, class DistanceFunc>
    std::enable_if_t<Level == Parallelism::OMP || Level == Parallelism::Hybrid> calculateAssignmentsFromCentroids(
      DistanceFunc& distanceFunc)
    {
        T cost = 0.0;

#pragma omp parallel for schedule(static), reduction(+ : cost)
        for (int32_t i = 0; i < p_data->rows(); ++i)
        {
            auto closestCentroid = findClosestCentroid(p_data->crowBegin(i), p_data->crowEnd(i), distanceFunc);
            m_assignments[i]     = closestCentroid.idx;
            cost += std::pow(closestCentroid.distance, 2);
        }

        m_error = cost;
    }

private:
    struct ClosestCentroid
    {
        int32_t idx;
        T distance;

        ClosestCentroid() : idx(-1), distance(std::numeric_limits<T>::max()) {}

        bool isGreaterThan(const T otherDist) const { return distance > otherDist; }

        void set(const int32_t idx, const T distance)
        {
            this->idx      = idx;
            this->distance = distance;
        }
    };

    template <typename Iter, class DistanceFunc>
    ClosestCentroid findClosestCentroid(Iter begin, Iter end, DistanceFunc& distanceFunc)
    {
        ClosestCentroid closestCentroid;

        for (int32_t i = 0; i < m_centroids.rows(); ++i)
        {
            auto tempDist = distanceFunc(begin, end, m_centroids.crowBegin(i), m_centroids.crowEnd(i));
            if (closestCentroid.isGreaterThan(tempDist))
                closestCentroid.set(i, tempDist);
        }

        return closestCentroid;
    }

private:
    T m_error;
    const Matrix<T>* p_data;
    DistanceMatrix<T>* p_distMat;
    SelectedSet m_selectedSet;
    std::vector<int32_t> m_assignments;
    Matrix<T> m_centroids;
};
}  // namespace hpkmedoids