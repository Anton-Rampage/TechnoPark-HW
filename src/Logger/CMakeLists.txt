add_library(logger STATIC Logger.cpp
                          BaseLogger.cpp
                          FileLogger.cpp
                          StdoutLogger.cpp
                          StderrLogger.cpp)
target_include_directories(logger PUBLIC ../../include/Logger)
target_compile_options(logger PUBLIC -O3)