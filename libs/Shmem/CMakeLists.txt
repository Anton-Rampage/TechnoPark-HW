add_library(shmem INTERFACE)
target_sources(shmem INTERFACE
        include/Map.h
        include/Shalloc.h
        )

target_include_directories(shmem INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )

target_link_libraries(shmem INTERFACE pthread)
target_compile_options(shmem INTERFACE -O3)
