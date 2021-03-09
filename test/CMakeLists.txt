cmake_minimum_required (VERSION 2.8)
project(elaine)

include_directories(.)
include_directories(../thirdparty)
include_directories(../src)

aux_source_directory(../thirdparty/libcontext libcontext_src)
aux_source_directory(../src src)


add_executable(liburing_test liburing_test.cpp)
target_link_libraries(liburing_test uring)

add_executable(fcontext_test fcontext_test.cpp ${libcontext_src})
target_link_libraries(fcontext_test)

add_executable(coroutine_test coroutine_test.cpp ${src} ${libcontext_src})
target_link_libraries(coroutine_test uring pthread)

add_executable(singleton_test singleton_test.cpp)

add_executable(multiplexer_test multiplexer_test.cpp ${src} ${libcontext_src})
target_link_libraries(multiplexer_test uring)

add_executable(defer_test defer_test.cpp ${src} ${libcontext_src})
target_link_libraries(defer_test pthread uring)

add_executable(worker_test worker_test.cpp ${src} ${libcontext_src})
target_link_libraries(worker_test pthread uring)

add_executable(scheduler_test scheduler_test.cpp ${src} ${libcontext_src})
target_link_libraries(scheduler_test pthread uring)

add_executable(sfinae_test sfinae_test.cpp ${src} ${libcontext_src})
target_link_libraries(sfinae_test pthread uring)
