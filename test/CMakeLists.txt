cmake_minimum_required (VERSION 2.8)
project(elaine)

include_directories(.)
include_directories(../thirdparty)
include_directories(../src)

add_executable(liburing_test liburing_test.cpp)
target_link_libraries(liburing_test uring)

add_executable(fcontext_test fcontext_test.cpp ../thirdparty/libcontext/libcontext.cpp)
target_link_libraries(fcontext_test)

add_executable(coroutine_test coroutine_test.cpp ../src/coroutine.cpp ../thirdparty/libcontext/libcontext.cpp)
target_link_libraries(coroutine_test pthread)