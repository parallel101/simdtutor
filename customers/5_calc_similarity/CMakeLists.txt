cmake_minimum_required(VERSION 3.18)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_STANDARD 20)

project(main LANGUAGES CXX)

add_executable(main avx2old.cpp)

find_package(OpenMP REQUIRED)
target_link_libraries(main OpenMP::OpenMP_CXX)
target_compile_options(main PUBLIC $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-mavx2$<SEMICOLON>-mfma> $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/arch:AVX2>)
