cmake_minimum_required(VERSION 3.13)
project(project)
set(CMAKE_CXX_STANDARD 17)

#lib process
add_library(process STATIC src/Process.cpp)
target_include_directories(process PUBLIC include)
target_compile_options(process PUBLIC -O3)

add_subdirectory(examples)


