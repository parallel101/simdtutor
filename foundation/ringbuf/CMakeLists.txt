cmake_minimum_required(VERSION 3.10)

project(ring_buffer CXX)

set(CMAKE_CXX_STANDARD 20)

add_executable(ring_buffer ring_buffer.cpp)

find_package(fmt REQUIRED)
target_link_libraries(ring_buffer fmt::fmt)

find_package(OpenMP REQUIRED)
target_link_libraries(ring_buffer OpenMP::OpenMP_CXX) # -fopenmp

find_package(TBB REQUIRED)
target_link_libraries(ring_buffer TBB::tbb)
