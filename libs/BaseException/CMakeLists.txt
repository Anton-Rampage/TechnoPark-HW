add_library(exception STATIC src/BaseException.cpp)
target_include_directories(exception PUBLIC include)
target_compile_options(exception PUBLIC -O0)
