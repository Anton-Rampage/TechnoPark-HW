#include "StdoutLogger.h"
#include <iostream>

namespace logger {
StdoutLogger::StdoutLogger(Level level) : BaseLogger(level) {}

void StdoutLogger::flush() {
    std::cout << std::flush;
}

void StdoutLogger::log(const std::string &str, Level log_level) {
    if (log_level <= level()) {
        std::cout << level_str[static_cast<int>(log_level) - 1] << ' ' << str << std::endl;
    }
}
}  // namespace logger
