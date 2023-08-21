cmake_minimum_required(VERSION 3.18)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_STANDARD 17)

project(main LANGUAGES CXX)

add_executable(main main.cpp)

find_package(benchmark REQUIRED)
target_link_libraries(main PUBLIC benchmark::benchmark)
target_link_libraries(main PUBLIC benchmark::benchmark_main)
target_compile_definitions(main PUBLIC -DBENCHMARK_DISPATCHED_KERNELS)
target_compile_options(main PUBLIC $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-mavx2$<SEMICOLON>-mfma>)
