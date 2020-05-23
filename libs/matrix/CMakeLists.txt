add_library(matrix STATIC src/matrix.cpp)

target_include_directories(matrix PUBLIC include
                                PRIVATE include/matrix)

define_debug_definitions(matrix)