add_library(process STATIC Process.cpp)
target_include_directories(process PUBLIC ../../include/Process)
target_compile_options(process PUBLIC -O3)