#include <hpkmediods/types/parallelism.hpp>
#include <iostream>

namespace hpkmediods
{
std::string parallelismToString(const Parallelism& parallelism)
{
    if (parallelism == Parallelism::Serial)
        return SERIAL;
    else if (parallelism == Parallelism::OMP)
        return OMP;
    else if (parallelism == Parallelism::MPI)
        return MPI;
    else if (parallelism == Parallelism::Hybrid)
        return HYBRID;
    else
        std::cerr << "Unknown Parallelism level!" << std::endl;

    exit(1);
}

Parallelism parallelismFromString(const std::string& strParallelism)
{
    if (strParallelism == SERIAL)
        return Parallelism::Serial;
    else if (strParallelism == OMP)
        return Parallelism::OMP;
    else if (strParallelism == MPI)
        return Parallelism::MPI;
    else if (strParallelism == HYBRID)
        return Parallelism::Hybrid;
    else
        std::cerr << "Unknown Parallelism level!" << std::endl;

    exit(1);
}
}  // namespace hpkmediods