add_library(process STATIC Process.cpp)
target_include_directories(process PUBLIC ${PROJECT_SOURCE_DIR}/include/Process)
target_compile_options(process PUBLIC -O3)
