cmake_minimum_required(VERSION 3.10)

project(decimal CXX)

add_executable(decimal decimal.cpp)

find_package(fmt REQUIRED)
target_link_libraries(decimal fmt::fmt)

find_package(OpenMP REQUIRED)
target_link_libraries(decimal OpenMP::OpenMP_CXX) # -fopenmp

find_package(TBB REQUIRED)
target_link_libraries(decimal TBB::tbb)

target_compile_options(decimal PRIVATE -march=native)
