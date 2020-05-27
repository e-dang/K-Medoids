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

    virtual T maximize(const Matrix<T>* const data, Matrix<T>* const centroids, std::vector<int32_t>* const assignments,
                       Matrix<T>* const dataDistMat, Matrix<T>* const centroidDistMat,
                       std::vector<int32_t>* const unselected, std::vector<int32_t>* const selected) const = 0;
};
}  // namespace hpkmediods