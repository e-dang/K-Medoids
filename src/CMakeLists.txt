find_package(MPI REQUIRED)
find_package(Boost 1.72.0 COMPONENTS random, timer)

add_library(hpkmediods types/parallelism.cpp
                       types/selected_set.cpp
                       types/clusters.cpp
                       types/distance_matrix.cpp
                       selectors/uniform_selectors.cpp
                       filesystem/file_rotator.cpp)

target_link_libraries(hpkmediods PUBLIC matrix ${Boost_LIBRARIES})