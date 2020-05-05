add_library(descriptor STATIC src/Descriptor.cpp)
target_include_directories(descriptor PUBLIC include)
target_compile_options(descriptor PUBLIC -O3)
