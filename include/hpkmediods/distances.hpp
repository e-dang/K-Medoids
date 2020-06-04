#pragma once

#include <cmath>
#include <numeric>

template <typename T>
struct L2Norm
{
    template <typename Iter1, typename Iter2>
    T operator()(const Iter1 p1Begin, const Iter1 p1End, const Iter2 p2Begin, const Iter2) const
    {
        auto result = std::inner_product(p1Begin, p1End, p2Begin, 0.0, std::plus<>(),
                                         [](const T val1, const T val2) { return std::pow(val1 - val2, 2); });

        return std::sqrt(result);
    }
};

template <typename T>
struct L1Norm
{
    template <typename Iter1, typename Iter2>
    T operator()(const Iter1 p1Begin, const Iter1 p1End, const Iter2 p2Begin, const Iter2) const
    {
        return std::inner_product(p1Begin, p1End, p2Begin, 0.0, std::plus<>(),
                                  [](const T val1, const T val2) { return std::abs(val1 - val2); });
    }
};