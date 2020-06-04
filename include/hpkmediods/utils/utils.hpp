#pragma once

#include <mpi.h>

#include <algorithm>
#include <array>
#include <iostream>
#include <limits>

namespace hpkmediods
{
template <typename Iter>
void print(Iter begin, Iter end)
{
    for (; begin != end; ++begin)
    {
        std::cout << *begin << " ";
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

template <typename Iter>
typename Iter::value_type getSecondLowest(Iter begin, Iter end)
{
    std::array<typename Iter::value_type, 2> temp;
    std::partial_sort_copy(begin, end, temp.begin(), temp.end());
    return temp[1];
}

template <typename T>
MPI_Datatype matchMPIType()
{
    MPI_Datatype dtype;
    MPI_Type_match_size(MPI_TYPECLASS_REAL, sizeof(T), &dtype);
    return dtype;
}
}  // namespace hpkmediods