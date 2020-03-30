#include "BaseLogger.h"

namespace logger {
BaseLogger::BaseLogger(Level level) : _level(level) {}

void BaseLogger::debug(const std::string &str) {
    log(str, Level::DEBUG);
}

void BaseLogger::info(const std::string &str) {
    log(str, Level::INFO);
}

void BaseLogger::warn(const std::string &str) {
    log(str, Level::WARN);
}

void BaseLogger::error(const std::string &str) {
    log(str, Level::ERROR);
}

void BaseLogger::set_level(Level level) {
    _level = level;
}

Level BaseLogger::level() {
    return _level;
}

LogException::LogException(std::string error) : _error(std::move(error)) {}

const char *LogException::what() const noexcept {
    return _error.c_str();
}
}  // namespace logger
