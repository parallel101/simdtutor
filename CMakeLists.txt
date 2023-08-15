cmake_minimum_required(VERSION 3.18)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_STANDARD 20)

project(main LANGUAGES CXX)

if (MSVC)
    add_compile_options(/arch:AVX2)    # 如果你的电脑不支持 AVX2，请删除本行
endif()

if (CMAKE_COMPILER_IS_GNUCC)
    add_compile_options(-mavx2 -mfma)  # 如果你的电脑不支持 AVX2，请删除本行
endif()

add_executable(main main.cpp)

find_package(benchmark REQUIRED)
target_link_libraries(main PUBLIC benchmark::benchmark)
target_link_libraries(main PUBLIC benchmark::benchmark_main)  # 测试性能时使用此行

find_package(GTest REQUIRED)
target_link_libraries(main PUBLIC GTest::gtest)
# target_link_libraries(main PUBLIC GTest::gtest_main)  # 测试正确性时使用此行

find_package(OpenMP REQUIRED)
target_link_libraries(main PUBLIC OpenMP::OpenMP_CXX)
