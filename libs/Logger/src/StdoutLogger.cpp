#include "StdoutLogger.h"

#include <chrono>
#include <iomanip>
#include <iostream>

namespace tp {
namespace logger {
StdoutLogger::StdoutLogger(Level level) : BaseLogger(level) {}

void StdoutLogger::flush() { std::cout << std::flush; }

void StdoutLogger::log(const std::string &str, Level log_level) {
    if (log_level <= level()) {
        auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::cout << "[" << std::put_time(std::localtime(&time), "%X") << "] "
                  << level_str[static_cast<int>(log_level) - 1] << ' ' << str << std::endl;
    }
}
}  // namespace logger
}  // namespace tp
