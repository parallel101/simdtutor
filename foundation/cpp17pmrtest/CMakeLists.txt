cmake_minimum_required(VERSION 3.18)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_STANDARD 20)

project(main LANGUAGES CXX)

add_executable(main main.cpp)

find_package(fmt REQUIRED)
target_link_libraries(main PUBLIC fmt::fmt)

find_package(TBB REQUIRED)
target_link_libraries(main PUBLIC TBB::tbb)

target_compile_options(main PUBLIC -mavx2 -mfma)
