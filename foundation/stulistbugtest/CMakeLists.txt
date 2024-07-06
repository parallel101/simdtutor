cmake_minimum_required(VERSION 3.18)

project(main)


execute_process(COMMAND llvm-config --libs core --system-libs
                RESULT_VARIABLE llvm_libs)
execute_process(COMMAND llvm-config --cxxflags
                RESULT_VARIABLE llvm_cxxflags)
execute_process(COMMAND llvm-config --ldflags
                RESULT_VARIABLE llvm_ldflags)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE ${llvm_libs})
target_compile_options(main PRIVATE ${llvm_cxxflags})
target_link_options(main PRIVATE ${llvm_ldflags})
