#pragma once

#include <hpkmediods/cost_calculator.hpp>
#include <hpkmediods/finders/centroid_finder.hpp>
#include <hpkmediods/maximizers/interface.hpp>
#include <iostream>
#include <string>
#include <type_traits>

namespace hpkmediods
{
const std::string PAM = "pam";

template <typename T, Parallelism Level, class DistanceFunc>
class PartitionAroundMediods : public IMaximizer<T>
{
public:
    T maximize(const Matrix<T>* const data, Matrix<T>& centroids, std::vector<int32_t>& assignments,
               std::vector<T>& sqDistances) const override
    {
        bool changed = true;

        do
        {
            changed = swapIteration(data, centroids, assignments, sqDistances);
            m_finder.findAndUpdateClosestCentroids(data, centroids, assignments, sqDistances);
        } while (changed);

        return m_costCalculator.calculateFromDistances(sqDistances);
    }

private:
    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::Serial || _Level == Parallelism::OMP, bool> swapIteration(
      const Matrix<T>* const data, Matrix<T>& centroids, std::vector<int32_t>& assignments,
      std::vector<T>& sqDistances) const
    {
        bool changed = false;

        for (int i = 0; i < centroids.rows(); ++i)
        {
            std::vector<T> tempStorage(centroids.crowBegin(i), centroids.crowEnd(i));
            auto bestCost   = m_costCalculator.calculateForSubset(i, data, centroids, assignments);
            int32_t bestIdx = -1;
            for (int32_t point = 0; point < data->rows(); ++point)
            {
                if (assignments[point] == i)
                {
                    centroids.set(i, data->crowBegin(point), data->crowEnd(point));
                    auto newCost = m_costCalculator.calculateForSubset(i, data, centroids, assignments);
                    if (newCost < bestCost)
                    {
                        bestCost = newCost;
                        bestIdx  = point;
                        changed  = true;
                    }
                }
            }

            applyBestConfiguration(i, bestIdx, data, centroids, tempStorage);
        }

        return changed;
    }

    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::MPI || _Level == Parallelism::Hybrid, bool> swapIteration(
      const Matrix<T>* const data, Matrix<T>& centroids, std::vector<int32_t>& assignments,
      std::vector<T>& sqDistances) const
    {
        bool changed = false;

#pragma omp parallel for shared(data, centroids, assignments, sqDistances, changed), schedule(static)
        for (int i = 0; i < centroids.rows(); ++i)
        {
            std::vector<T> tempStorage(centroids.crowBegin(i), centroids.crowEnd(i));
            auto bestCost   = m_costCalculator.calculateForSubset(i, data, centroids, assignments);
            int32_t bestIdx = -1;
            for (int32_t point = 0; point < data->rows(); ++point)
            {
                if (assignments[point] == i)
                {
                    centroids.set(i, data->crowBegin(point), data->crowEnd(point));
                    auto newCost = m_costCalculator.calculateForSubset(i, data, centroids, assignments);
                    if (newCost < bestCost)
                    {
                        bestCost = newCost;
                        bestIdx  = point;
                        changed  = true;
                    }
                }
            }

            applyBestConfiguration(i, bestIdx, data, centroids, tempStorage);
        }

        return changed;
    }

    void applyBestConfiguration(const int centroidIdx, const int32_t bestIdx, const Matrix<T>* const data,
                                Matrix<T>& centroids, std::vector<T>& tempStorage) const
    {
        if (bestIdx == -1)
            centroids.set(centroidIdx, tempStorage.cbegin(), tempStorage.cend());
        else
            centroids.set(centroidIdx, data->crowBegin(bestIdx), data->crowEnd(bestIdx));
    }

private:
    CostCalculator<T, Level, DistanceFunc> m_costCalculator;
    CentroidFinder<T, Level, DistanceFunc> m_finder;
};
}  // namespace hpkmediods