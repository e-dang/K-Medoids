#pragma once

#include <hpkmediods/types/parallelism.hpp>
#include <matrix/matrix.hpp>
#include <vector>

namespace hpkmediods
{
template <typename T>
class IMaximizer
{
public:
    virtual ~IMaximizer() = default;

    virtual T maximize(const Matrix<T>* const data, Matrix<T>& centroids, std::vector<int32_t>& assignments,
                       std::vector<T>& sqDistances) const = 0;
};
}  // namespace hpkmediods