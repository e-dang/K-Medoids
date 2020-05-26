find_package(MPI REQUIRED)
find_package(Boost 1.72.0 COMPONENTS random)

add_library(hpkmediods types/parallelism.cpp
                       types/cluster_data.cpp
                       selectors/uniform_selectors.cpp
                       filesystem/file_rotator.cpp)

target_link_libraries(hpkmediods PUBLIC matrix ${Boost_LIBRARIES})