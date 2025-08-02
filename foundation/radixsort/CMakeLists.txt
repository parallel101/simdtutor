cmake_minimum_required(VERSION 3.10)

project(radix_sort CXX)

add_executable(radix_sort radix_sort.cpp)

find_package(fmt REQUIRED)
target_link_libraries(radix_sort fmt::fmt)

find_package(OpenMP REQUIRED)
target_link_libraries(radix_sort OpenMP::OpenMP_CXX) # -fopenmp

find_package(TBB REQUIRED)
target_link_libraries(radix_sort TBB::tbb)
