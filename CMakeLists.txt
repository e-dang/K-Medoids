cmake_minimum_required(VERSION 3.0.0)
project(K-Mediods VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    SET(CMAKE_CXX_FLAGS "-Wall -Wpedantic -fopenmp")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    SET(CMAKE_CXX_FLAGS "-Wall -Wpedantic -fopenmp -L/usr/lib64 -lstdc++ -lm")
endif()

cmake_policy(SET CMP0064 NEW) # allow TEST operator in 'if' statements

# top level function for debug compile definitions
function(define_debug_definitions TARGET)
    if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR TEST TARGET)
        target_compile_definitions(${TARGET} PUBLIC DEBUG)
    endif()
endfunction()

include_directories(include)

add_subdirectory(src)
add_subdirectory(libs)
add_subdirectory(tests)

add_executable(kmediods_serial_reg src/main.cpp)
add_executable(kmediods_serial_clara src/main.cpp)
add_executable(kmediods_omp_reg src/main.cpp)
add_executable(kmediods_omp_clara src/main.cpp)
add_executable(kmediods_mpi_reg src/main.cpp)
add_executable(kmediods_mpi_clara src/main.cpp)
add_executable(kmediods_hybrid_reg src/main.cpp)
add_executable(kmediods_hybrid_clara src/main.cpp)

target_link_libraries(kmediods_serial_reg hpkmediods)
target_link_libraries(kmediods_serial_clara hpkmediods)
target_link_libraries(kmediods_omp_reg hpkmediods)
target_link_libraries(kmediods_omp_clara hpkmediods)
target_link_libraries(kmediods_mpi_reg hpkmediods)
target_link_libraries(kmediods_mpi_clara hpkmediods)
target_link_libraries(kmediods_hybrid_reg hpkmediods)
target_link_libraries(kmediods_hybrid_clara hpkmediods)

target_compile_definitions(kmediods_serial_reg PUBLIC METHOD="REG" PARALLELISM="serial")
target_compile_definitions(kmediods_serial_clara PUBLIC METHOD="CLARA" PARALLELISM="serial")
target_compile_definitions(kmediods_omp_reg PUBLIC METHOD="REG" PARALLELISM="omp")
target_compile_definitions(kmediods_omp_clara PUBLIC METHOD="CLARA" PARALLELISM="omp")
target_compile_definitions(kmediods_mpi_reg PUBLIC METHOD="REG" PARALLELISM="mpi")
target_compile_definitions(kmediods_mpi_clara PUBLIC METHOD="CLARA" PARALLELISM="mpi")
target_compile_definitions(kmediods_hybrid_reg PUBLIC METHOD="REG" PARALLELISM="hybrid")
target_compile_definitions(kmediods_hybrid_clara PUBLIC METHOD="CLARA" PARALLELISM="hybrid")

message("BUILD TYPE: " ${CMAKE_BUILD_TYPE})