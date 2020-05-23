cmake_minimum_required(VERSION 3.0.0)
project(K-Mediods VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 14)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

# top level function for debug compile definitions
function(define_debug_definitions TARGET)
    if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        target_compile_definitions(${TARGET} PUBLIC DEBUG)
    endif()
endfunction()

include_directories(include)

add_subdirectory(src)
add_subdirectory(libs)
add_subdirectory(tests)