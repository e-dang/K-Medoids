#pragma once

#include <hpkmedoids/types/clusters.hpp>
#include <hpkmedoids/types/distance_matrix.hpp>
#include <hpkmedoids/types/selected_set.hpp>
#include <matrix/matrix.hpp>

namespace hpkmedoids
{
template <typename T>
class IInitializer
{
public:
    virtual ~IInitializer() = default;

    virtual void initialize(const Matrix<T>* const data, Clusters<T>* const clusters,
                            const DistanceMatrix<T>* const distMat) const = 0;
};
}  // namespace hpkmedoids