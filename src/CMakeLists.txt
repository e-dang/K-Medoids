find_package(MPI REQUIRED)
find_package(Boost 1.71.0 COMPONENTS random, timer)

add_library(hpkmedoids types/parallelism.cpp
                       types/selected_set.cpp
                       types/clusters.cpp
                       types/distance_matrix.cpp
                       utils/uniform_selectors.cpp
                       filesystem/file_rotator.cpp)

target_link_libraries(hpkmedoids PUBLIC matrix ${MPI_LIBRARIES} ${Boost_LIBRARIES})