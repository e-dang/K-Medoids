find_package(MPI REQUIRED)
find_package(Boost 1.72.0 COMPONENTS random)

add_library(hpkmediods types/parallelism.cpp)