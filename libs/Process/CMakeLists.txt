add_library(process STATIC src/Process.cpp)
target_include_directories(process PUBLIC include)
target_link_libraries(process descriptor exception)
target_compile_options(process PUBLIC -O3)
