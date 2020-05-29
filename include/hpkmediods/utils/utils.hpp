#pragma once

#include <mpi.h>

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
    typename Iter::value_type secondLowest = std::numeric_limits<typename Iter::value_type>::max() - 1;
    typename Iter::value_type lowest       = std::numeric_limits<typename Iter::value_type>::max();
    for (; begin != end; ++begin)
    {
        if (*begin > lowest)
        {
            secondLowest = lowest;
            lowest       = *begin;
        }
        else if (*begin > secondLowest && *begin != lowest)
        {
            secondLowest = *begin;
        }
    }

    return secondLowest;
}

template <typename T>
MPI_Datatype matchMPIType()
{
    MPI_Datatype dtype;
    MPI_Type_match_size(MPI_TYPECLASS_REAL, sizeof(T), &dtype);
    return dtype;
}
}  // namespace hpkmediods