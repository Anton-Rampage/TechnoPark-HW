#include "Logger.h"
#include "FileLogger.h"
#include "StderrLogger.h"
#include "StdoutLogger.h"

namespace logger {
Logger &Logger::get_instance() {
    static Logger instance{};
    return instance;
}

std::unique_ptr<BaseLogger>& Logger::get_global_logger() {
    return _global_logger;
}

void Logger::set_global_logger(std::unique_ptr<BaseLogger> new_logger) {
    _global_logger = std::move(new_logger);
}

Logger::Logger() {
    _global_logger = create_stdout_logger(Level::WARN);
}

void debug(const std::string& str) {
    Logger::get_instance().get_global_logger()->debug(str);
}

void info(const std::string& str) {
    Logger::get_instance().get_global_logger()->info(str);
}

void warn(const std::string& str) {
    Logger::get_instance().get_global_logger()->warn(str);
}

void error(const std::string& str) {
    Logger::get_instance().get_global_logger()->error(str);
}

std::unique_ptr<BaseLogger> create_file_logger(Level level, const std::string& filename) {
    return std::make_unique<FileLogger>(level, filename);
}

std::unique_ptr<BaseLogger> create_stdout_logger(Level level) {
    return std::make_unique<StdoutLogger>(level);
}

std::unique_ptr<BaseLogger> create_stderr_logger(Level level) {
    return std::make_unique<StderrLogger>(level);
}
}  // namespace logger
