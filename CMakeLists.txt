cmake_minimum_required (VERSION 2.8)
project(elaine)

set(CMAKE_CXX_FLAGS "-pthread -Wall -g -ggdb ${CMAKE_CXX_FLAGS}")

add_subdirectory(test)
add_subdirectory(src)