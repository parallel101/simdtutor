cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)

project(lock_free_queue CXX)

add_executable(lock_free_queue lock_free_queue.cpp)

find_package(fmt REQUIRED)
target_link_libraries(lock_free_queue fmt::fmt)

find_package(OpenMP REQUIRED)
target_link_libraries(lock_free_queue OpenMP::OpenMP_CXX)

find_package(TBB REQUIRED)
target_link_libraries(lock_free_queue TBB::tbb)
