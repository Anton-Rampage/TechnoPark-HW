set(SRC_FILES src/Logger.cpp
              src/BaseLogger.cpp
              src/FileLogger.cpp
              src/StdoutLogger.cpp
              src/StderrLogger.cpp)

add_library(logger STATIC ${SRC_FILES})
target_include_directories(logger PUBLIC include)
target_link_libraries(logger exception)
target_compile_options(logger PUBLIC -O3)
