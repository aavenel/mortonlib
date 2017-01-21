cmake_minimum_required (VERSION 2.6)
project(morton_arithmetic)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

add_subdirectory(include)
add_subdirectory(tests)

