add_library(tcp STATIC src/Server.cpp src/Connection.cpp)
target_include_directories(tcp PUBLIC include)
target_compile_options(tcp PUBLIC -O3)
target_link_libraries(tcp descriptor exception)
