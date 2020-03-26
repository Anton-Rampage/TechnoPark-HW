#process_example
add_executable(process_example ex.cpp)
target_link_libraries(process_example process)
add_executable(echo echo.cpp)

#log_example
add_executable(log_example log_example.cpp)
target_link_libraries(log_example logger)
