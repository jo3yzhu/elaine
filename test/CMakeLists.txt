cmake_minimum_required (VERSION 2.8)
project(elaine)

include_directories(.)
include_directories(../../thirdparty/libcontext)

add_executable(liburing_test liburing_test.cpp)
target_link_libraries(liburing_test uring)

set(CMAKE_CXX_FLAGS "-static -Wall")
add_executable(fcontext_test fcontext_test.cpp ../../thirdparty/libcontext/libcontext.cpp)
