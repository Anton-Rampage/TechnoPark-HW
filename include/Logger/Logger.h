#ifndef INCLUDE_LOGGER_H_
#define INCLUDE_LOGGER_H_

#include "BaseLogger.h"
#include <string>
#include <memory>
#include <fstream>
#include <exception>

namespace logger {

void debug(const std::string& str);
void info(const std::string& str);
void warn(const std::string& str);
void error(const std::string& str);


class Logger {
 public:
    static Logger& get_instance();
    std::unique_ptr<BaseLogger>& get_global_logger();
    void set_global_logger(std::unique_ptr<BaseLogger> new_logger);

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
 private:
    Logger();
    ~Logger() = default;

    std::unique_ptr<BaseLogger> _global_logger;
};

std::unique_ptr<BaseLogger> create_file_logger(Level level, const std::string& filename = "log.txt");

std::unique_ptr<BaseLogger> create_stdout_logger(Level level);

std::unique_ptr<BaseLogger> create_stderr_logger(Level level);
}  // namespace logger

#endif  // INCLUDE_LOGGER_H_
