#pragma once

#include <hpkmediods/types/parallelism.hpp>
#include <type_traits>
#include <vector>

namespace hpkmediods
{
template <typename T, Parallelism Level, class DistanceFunc>
class CostCalculator
{
public:
    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::Serial || _Level == Parallelism::MPI, T> calculateFromDistances(
      const std::vector<T>& sqDistances) const
    {
        return std::accumulate(sqDistances.cbegin(), sqDistances.cend(), 0.0);
    }

    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::OMP || _Level == Parallelism::Hybrid, T> calculateFromDistances(
      const std::vector<T>& sqDistances) const
    {
        T cost = 0.0;

#pragma omp parallel for shared(sqDistances), schedule(static), reduction(+ : cost)
        for (int i = 0; i < static_cast<int>(sqDistances.size()); ++i)
        {
            cost += sqDistances[i];
        }

        return cost;
    }

    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::Serial || _Level == Parallelism::MPI, T> calculateForSubset(
      const int centroidIdx, const Matrix<T>* const data, const Matrix<T>& centroids,
      const std::vector<int32_t>& assignments) const
    {
        T cost             = 0.0;
        auto centroidBegin = centroids.crowBegin(centroidIdx);
        auto centroidEnd   = centroids.crowEnd(centroidIdx);

        for (int i = 0; i < data->rows(); ++i)
        {
            if (assignments[i] == centroidIdx)
            {
                cost += m_distanceFunc(data->crowBegin(i), data->crowEnd(i), centroidBegin, centroidEnd);
            }
        }

        return cost;
    }

    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::OMP || _Level == Parallelism::Hybrid, T> calculateForSubset(
      const int centroidIdx, const Matrix<T>* const data, const Matrix<T>& centroids,
      const std::vector<int32_t>& assignments) const
    {
        T cost = 0.0;
#pragma omp parallel for shared(centroidIdx, data, centroids, assignments), schedule(static), reduction(+ : cost)
        for (int i = 0; i < data->rows(); ++i)
        {
            if (assignments[i] == centroidIdx)
            {
                cost += m_distanceFunc(data->crowBegin(i), data->crowEnd(i), centroids.crowBegin(centroidIdx),
                                       centroids.crowEnd(centroidIdx));
            }
        }

        return cost;
    }

private:
    DistanceFunc m_distanceFunc;
};
}  // namespace hpkmediods