add_library(epoll STATIC src/EpollServer.cpp src/EpollConnection.cpp)
target_include_directories(epoll PUBLIC include)
target_compile_options(epoll PUBLIC -O3)
target_link_libraries(epoll descriptor exception)
