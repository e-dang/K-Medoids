#pragma once

#include <cmath>
// #include <functional>
#include <numeric>

template <typename T>
struct L2Norm
{
    template <typename Iter>
    T operator()(const Iter p1Begin, const Iter p1End, const Iter p2Begin, const Iter)
    {
        auto result = std::inner_product(p1Begin, p1End, p2Begin, 0.0, std::plus<>(),
                                         [](const T val1, const T val2) { return std::pow(val1 - val2, 2); });

        return std::sqrt(result);
    }
};