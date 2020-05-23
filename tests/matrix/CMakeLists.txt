add_executable(test_matrix test_matrix.cpp)

target_link_libraries(test_matrix matrix ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(NAME test_matrix COMMAND test_matrix)
