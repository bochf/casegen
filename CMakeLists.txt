cmake_minimum_required (VERSION 3.22)
project (casegen)

add_subdirectory(libs)
add_subdirectory(src)

find_package(GTest REQUIRED)
include(CTest)
add_subdirectory(gtest)
enable_testing()
