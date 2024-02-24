aux_source_directory(. test_src)
add_executable(test_traveller ${test_src})
target_link_libraries(test_traveller traveller)
target_link_libraries(test_traveller GTest::gtest_main)

add_test(NAME test_traveller COMMAND test_traveller)
file(COPY test_matrix.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
