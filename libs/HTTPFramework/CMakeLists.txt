add_library(http STATIC src/Server.cpp src/Connection.cpp src/HttpRequest.cpp src/HttpResponse.cpp)
target_include_directories(http PUBLIC include)
target_link_libraries(http coroutine logger descriptor exception pthread)
target_compile_options(http PUBLIC -O0)
