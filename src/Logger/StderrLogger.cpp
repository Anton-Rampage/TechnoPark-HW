#include "StderrLogger.h"
#include <iostream>

namespace logger {
StderrLogger::StderrLogger(Level level) : BaseLogger(level) {}

void StderrLogger::flush() {
    std::cerr << std::flush;
}

void StderrLogger::log(const std::string &str, Level log_level) {
    if (log_level <= level()) {
        std::cerr << level_str[static_cast<int>(log_level) - 1] << ' ' << str << std::endl;
    }
}
}  // namespace logger
