add_library(shmem INTERFACE)
target_sources(shmem INTERFACE
        Map.h
        Shalloc.h
        )

target_include_directories(shmem INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/shmem>
        )

target_link_libraries(shmem INTERFACE pthread)
target_compile_options(shmem INTERFACE -O3)
