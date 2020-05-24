#pragma once

#include <matrix/matrix.hpp>
#include <vector>

namespace hpkmediods
{
template <typename T>
class IInitializer
{
public:
    virtual T initialize(const Matrix<T>* const data, Matrix<T>& centroids, std::vector<int32_t>& assignments,
                         std::vector<T>& sqDistances) const = 0;
};
}  // namespace hpkmediods