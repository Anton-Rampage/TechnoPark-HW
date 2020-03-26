#include <Logger.h>
#include <iostream>

namespace logger {

Logger &Logger::get_instance() {
    static Logger instance{};
    return instance;
}

std::shared_ptr<BaseLogger> Logger::get_global_logger() {
    return _global_logger;
}

void Logger::set_global_logger(std::shared_ptr<BaseLogger> new_logger) {
    _global_logger = std::move(new_logger);
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

std::shared_ptr<BaseLogger> create_file_logger(Level level, const std::string& filename) {
    return std::make_shared<FileLogger>(level, filename);
}

std::shared_ptr<BaseLogger> create_stdout_logger(Level level) {
    return std::make_shared<StdoutLogger>(level);
}

std::shared_ptr<BaseLogger> create_stderr_logger(Level level) {
    return std::make_shared<StderrLogger>(level);
}

BaseLogger::BaseLogger(Level level) : _level(level) {}

void BaseLogger::debug(const std::string &str) {
    log(str, DEBUG);
}

void BaseLogger::info(const std::string &str) {
    log(str, INFO);
}

void BaseLogger::warn(const std::string &str) {
    log(str, WARN);
}

void BaseLogger::error(const std::string &str) {
    log(str, ERROR);
}

void BaseLogger::set_level(Level level) {
    _level = level;
}

Level BaseLogger::level() {
    return _level;
}

FileLogger::FileLogger(Level level, const std::string &filename)
    : BaseLogger(level),
    _log_file(filename) {
    if (!_log_file.is_open()) {
        throw LogException("file not opened");
    }
}

void FileLogger::log(const std::string &str, Level log_level) {
    if (log_level <= level()) {
        _log_file << str << std::endl;
    }
}

void FileLogger::flush() {
    _log_file << std::flush;
}

FileLogger::~FileLogger() {
    _log_file.close();
}

StdoutLogger::StdoutLogger(Level level) : BaseLogger(level) {}

void StdoutLogger::flush() {
    std::cout << std::flush;
}

void StdoutLogger::log(const std::string &str, Level log_level) {
    if (log_level <= level()) {
        std::cout << str << std::endl;
    }
}

StderrLogger::StderrLogger(Level level) : BaseLogger(level) {}

void StderrLogger::flush() {
    std::cerr << std::flush;
}

void StderrLogger::log(const std::string &str, Level log_level) {
    if (log_level <= level()) {
        std::cerr << str << std::endl;
    }
}

LogException::LogException(std::string error) : _error(std::move(error)) {}

const char *LogException::what() const noexcept {
    return _error.c_str();
}
}
