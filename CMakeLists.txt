cmake_minimum_required (VERSION 2.6)
project(morton_arithmetic)

add_subdirectory(include)

add_executable(morton_test tests/tests.cpp)
target_link_libraries(morton_test mortonlib)