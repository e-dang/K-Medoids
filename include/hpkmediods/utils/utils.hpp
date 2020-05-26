#pragma once

#include <iostream>

namespace hpkmediods
{
template <typename Iter>
void print(Iter begin, Iter end)
{
    for (; begin != end; ++begin)
    {
        std::cout << begin << " ";
    }
    std::cout << '\n' << std::flush;
}

template <class ForwardIt>
ForwardIt min_element(ForwardIt first, ForwardIt last)
{
    ForwardIt smallest = first;
    ++first;
    for (; first != last; ++first)
    {
        if (*first < *smallest)
        {
            smallest = first;
        }
    }
    return smallest;
}
}  // namespace hpkmediods