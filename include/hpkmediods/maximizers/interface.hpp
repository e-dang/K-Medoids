#pragma once

#include <hpkmediods/types/clusters.hpp>
#include <hpkmediods/types/distance_matrix.hpp>
#include <hpkmediods/types/selected_set.hpp>
#include <matrix/matrix.hpp>

namespace hpkmediods
{
template <typename T>
class IMaximizer
{
public:
    virtual ~IMaximizer() = default;

    virtual void maximize(const Matrix<T>* const data, Clusters<T>* const clusters,
                          DistanceMatrix<T>* const distMat) const = 0;
};
}  // namespace hpkmediods