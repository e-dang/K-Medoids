#pragma once

#include <string>

namespace hpkmediods
{
const std::string SERIAL = "serial";
const std::string OMP    = "omp";
const std::string MPI    = "mpi";
const std::string HYBRID = "hybrid";

enum class Parallelism
{
    Serial,
    OMP,
    MPI,
    Hybrid
};

std::string parallelismToString(const Parallelism& parallelism);

Parallelism parallelismFromString(const std::string& strParallelism);
}  // namespace hpkmediods