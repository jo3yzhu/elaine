cmake_minimum_required (VERSION 2.8)
project(uring_test)

include_directories(.)

add_executable(uring_test main.cpp related_research.cpp)
target_link_libraries(uring_test uring)