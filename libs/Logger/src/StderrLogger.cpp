#include "StderrLogger.h"

#include <chrono>
#include <iomanip>
#include <iostream>

namespace tp {
namespace logger {
StderrLogger::StderrLogger(Level level) : BaseLogger(level) {}

void StderrLogger::flush() { std::cerr << std::flush; }

void StderrLogger::log(const std::string &str, Level log_level) {
    if (log_level <= level()) {
        auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::cerr << "[" << std::put_time(std::localtime(&time), "%X") << "] "
                  << level_str[static_cast<int>(log_level) - 1] << ' ' << str << std::endl;
    }
}
}  // namespace logger
}  // namespace tp
