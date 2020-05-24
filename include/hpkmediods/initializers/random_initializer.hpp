#pragma once

#include <hpkmediods/finders/centroid_finder.hpp>
#include <hpkmediods/initializers/interface.hpp>
#include <hpkmediods/selectors/uniform_selectors.hpp>

namespace hpkmediods
{
const std::string RANDOM_INITIALIZER = "random";

template <typename T, Parallelism Level, class DistanceFunc>
class RandomInitializer : public IInitializer<T>
{
public:
    T initialize(const Matrix<T>* const data, Matrix<T>& centroids, std::vector<int32_t>& assignments,
                 std::vector<T>& sqDistances) const override
    {
        selectCentroids(data, centroids);
        m_finder.findAndUpdateClosestCentroids(data, centroids, assignments, sqDistances);
        return std::accumulate(sqDistances.begin(), sqDistances.end(), 0.0);
    }

private:
    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::Serial || _Level == Parallelism::MPI> selectCentroids(
      const Matrix<T>* const data, Matrix<T>& centroids) const
    {
        auto selections = m_selector.select(centroids.rows(), data->rows());
        for (const auto& selection : selections)
        {
            centroids.append(data->crowBegin(selection), data->crowEnd(selection));
        }
    }

    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::OMP || _Level == Parallelism::Hybrid> selectCentroids(
      const Matrix<T>* const data, Matrix<T>& centroids) const
    {
        centroids.resize(centroids.rows());
        auto selections = m_selector.select(centroids.rows(), data->rows());
        auto selection  = selections.begin();

#pragma omp parallel for shared(data, centroids, selections), schedule(static)
        for (int i = 0; i < static_cast<int>(selections.size()); ++i, ++selection)
        {
            centroids.set(i, data->crowBegin(*selection), data->crowEnd(*selection));
        }
    }

private:
    UniformSelector m_selector;
    CentroidFinder<T, Level, DistanceFunc> m_finder;
};
}  // namespace hpkmediods