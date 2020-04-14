cmake_minimum_required(VERSION 3.13)
project(project)
set(CMAKE_CXX_STANDARD 17)

#lib process
add_subdirectory(src/Process)

#lib log
add_subdirectory(src/Logger)

#lib tcp
add_subdirectory(src/tcp)

#examples
add_subdirectory(examples)

#shmem
add_subdirectory(include/shmem)
