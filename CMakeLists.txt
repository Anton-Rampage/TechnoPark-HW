cmake_minimum_required(VERSION 3.16)
project(TechnoPark)
set(CMAKE_CXX_STANDARD 17)
#libs
add_subdirectory(libs)
#examples
add_subdirectory(examples)

#shmem
add_subdirectory(include/shmem)
