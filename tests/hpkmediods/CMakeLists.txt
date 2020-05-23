
add_executable(test_distances test_distances.cpp)

target_link_libraries(test_distances ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(NAME test_distances COMMAND test_distances)

add_subdirectory(types)
add_subdirectory(selectors)