add_library(coroutine STATIC src/Coroutine.cpp)
target_include_directories(coroutine PUBLIC include)
target_compile_options(coroutine PUBLIC -O0)
