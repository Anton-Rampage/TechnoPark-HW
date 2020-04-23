add_library(tcp STATIC Connection.cpp Server.cpp EpollServer.cpp)
target_include_directories(tcp PUBLIC ${PROJECT_SOURCE_DIR}/include/tcp)
target_compile_options(tcp PUBLIC -O3)
target_link_libraries(tcp process)
