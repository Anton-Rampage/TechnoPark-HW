cmake_minimum_required(VERSION 3.13)
project(project)
set(CMAKE_CXX_STANDARD 17)

#lib process
add_subdirectory(src/Process)

#lib log
add_subdirectory(src/Logger)

#examples
add_subdirectory(examples)
