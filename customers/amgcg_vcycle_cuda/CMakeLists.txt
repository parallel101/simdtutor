cmake_minimum_required(VERSION 3.10)

project(my_project LANGUAGES CXX CUDA)

set(SOURCES vcycle.cu)

add_library(fast-vcycle-gpu SHARED ${SOURCES})
target_include_directories(fast-vcycle-gpu PRIVATE include)
target_link_libraries(fast-vcycle-gpu PRIVATE cusparse cublas)

#add_executable(start_cloth3d start_cloth3d.cpp)
#add_dependencies(start_cloth3d fast-vcycle-gpu)
