#process_example
add_executable(process_example process_example.cpp)
target_link_libraries(process_example process)
add_executable(echo echo.cpp)

#log_example
add_executable(log_example log_example.cpp LogSettings.cpp)
target_link_libraries(log_example logger)

#tcp_example
add_executable(tcp_server tcp_server.cpp)
target_link_libraries(tcp_server tcp logger)

add_executable(tcp_client tcp_client.cpp)
target_link_libraries(tcp_client tcp logger)

add_executable(epoll_server epoll_tcp_example.cpp)
target_link_libraries(epoll_server epoll logger)

add_executable(epoll_client client_for_epoll.cpp)
target_link_libraries(epoll_client tcp logger)

#shmem_example
add_executable(shmem_example shmem_example.cpp)
target_link_libraries(shmem_example shmem logger exception)

