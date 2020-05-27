#pragma once

#include <hpkmediods/utils/utils.hpp>
#include <matrix/matrix.hpp>
#include <type_traits>
#include <vector>

namespace hpkmediods
{
template <typename T, Parallelism Level>
class ClusteringUpdater
{
public:
    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::Serial || _Level == Parallelism::MPI> updateClusteringFromDistMat(
      const Matrix<T>* const centroidDistMat, std::vector<int32_t>* const assignments) const
    {
        for (int i = 0; i < static_cast<int32_t>(assignments->size()); ++i)
        {
            auto distIter      = min_element(centroidDistMat->crowBegin(i), centroidDistMat->crowEnd(i));
            assignments->at(i) = std::distance(centroidDistMat->crowBegin(i), distIter);
        }
    }

    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::OMP || _Level == Parallelism::Hybrid> updateClusteringFromDistMat(
      const Matrix<T>* const centroidDistMat, std::vector<int32_t>* const assignments) const
    {
#pragma omp parallel for shared(centroidDistMat, assignments), schedule(static)
        for (int i = 0; i < static_cast<int32_t>(assignments->size()); ++i)
        {
            auto distIter      = min_element(centroidDistMat->crowBegin(i), centroidDistMat->crowEnd(i));
            assignments->at(i) = std::distance(centroidDistMat->crowBegin(i), distIter);
        }
    }
};
}  // namespace hpkmediods