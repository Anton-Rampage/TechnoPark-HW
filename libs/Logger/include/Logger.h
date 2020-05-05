#ifndef INCLUDE_LOGGER_LOGGER_H_
#define INCLUDE_LOGGER_LOGGER_H_

#include "BaseLogger.h"

#include <memory>
#include <string>

namespace tp {
namespace logger {

void debug(const std::string& str);
void info(const std::string& str);
void warn(const std::string& str);
void error(const std::string& str);

using BaseLoggerPtr = std::unique_ptr<BaseLogger>;

class Logger {
 public:
    static Logger& get_instance();
    BaseLogger& get_global_logger();
    void set_global_logger(BaseLoggerPtr new_logger);

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

 private:
    Logger();
    ~Logger() = default;

    BaseLoggerPtr _global_logger;
};

BaseLoggerPtr create_file_logger(Level level, const std::string& filename = "log.txt");

BaseLoggerPtr create_stdout_logger(Level level);

BaseLoggerPtr create_stderr_logger(Level level);
}  // namespace logger
}  // namespace tp

#endif  // INCLUDE_LOGGER_LOGGER_H_
