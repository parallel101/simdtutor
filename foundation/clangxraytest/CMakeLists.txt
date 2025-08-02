cmake_minimum_required(VERSION 3.18)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER clang++)

project(main LANGUAGES CXX)

add_executable(main main.cpp)

target_compile_options(main PRIVATE
    -fxray-instrument -fxray-instruction-threshold=1
)
target_link_options(main PRIVATE
    -fxray-instrument
)
