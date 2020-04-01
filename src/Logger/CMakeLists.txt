add_library(logger STATIC Logger.cpp
                          BaseLogger.cpp
                          FileLogger.cpp
                          StdoutLogger.cpp
                          StderrLogger.cpp)
target_include_directories(logger PUBLIC ${PROJECT_SOURCE_DIR}/include/Logger)
target_compile_options(logger PUBLIC -O3)