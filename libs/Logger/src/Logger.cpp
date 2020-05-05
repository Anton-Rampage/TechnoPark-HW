#include "Logger.h"

#include "BaseException.h"
#include "FileLogger.h"
#include "StderrLogger.h"
#include "StdoutLogger.h"

namespace tp {
namespace logger {
Logger& Logger::get_instance() {
    static Logger instance{};
    return instance;
}

BaseLogger& Logger::get_global_logger() {
    if (_global_logger == nullptr) {
        throw BaseException{"_global_logger have nullptr value"};
    }
    return *_global_logger;
}

void Logger::set_global_logger(BaseLoggerPtr new_logger) { _global_logger = std::move(new_logger); }

Logger::Logger() : _global_logger(create_stdout_logger(Level::WARN)) {}

void debug(const std::string& str) { Logger::get_instance().get_global_logger().debug(str); }

void info(const std::string& str) { Logger::get_instance().get_global_logger().info(str); }

void warn(const std::string& str) { Logger::get_instance().get_global_logger().warn(str); }

void error(const std::string& str) { Logger::get_instance().get_global_logger().error(str); }

BaseLoggerPtr create_file_logger(Level level, const std::string& filename) {
    return std::make_unique<FileLogger>(level, filename);
}

BaseLoggerPtr create_stdout_logger(Level level) { return std::make_unique<StdoutLogger>(level); }

BaseLoggerPtr create_stderr_logger(Level level) { return std::make_unique<StderrLogger>(level); }
}  // namespace logger
}  // namespace tp
