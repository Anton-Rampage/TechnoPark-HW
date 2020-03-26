#ifndef INCLUDE_LOGGER_H_
#define INCLUDE_LOGGER_H_

#include <string>
#include <memory>
#include <fstream>
#include <exception>

namespace logger {
class LogException : public std::exception {
 public:
    explicit LogException(std::string error);
    const char* what() const noexcept override;
 private:
    std::string _error;
};


void debug(const std::string& str);
void info(const std::string& str);
void warn(const std::string& str);
void error(const std::string& str);


class BaseLogger;

enum Level : int {
    DEBUG = 4,
    INFO = 3,
    WARN = 2,
    ERROR = 1
};


class Logger {
 public:
    static Logger& get_instance();
    std::shared_ptr<BaseLogger> get_global_logger();
    void set_global_logger(std::shared_ptr<BaseLogger> new_logger);

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
 private:
    Logger() = default;
    ~Logger() = default;

    std::shared_ptr<BaseLogger> _global_logger;
};


class BaseLogger {
 public:
    explicit BaseLogger(Level level);
    ~BaseLogger() = default;

    void debug(const std::string& str);
    void info(const std::string& str);
    void warn(const std::string& str);
    void error(const std::string& str);

    void set_level(Level level);
    Level level();

    virtual void flush() = 0;
 private:
    virtual void log(const std::string& str, Level level) = 0;

    Level _level;
};


class FileLogger : public BaseLogger {
 public:
    FileLogger(Level level, const std::string& filename);
    ~FileLogger();
    void flush() override;
 private:
    void log(const std::string &str, Level log_level) override;

    std::ofstream _log_file;
};


class StdoutLogger : public BaseLogger {
 public:
    explicit StdoutLogger(Level level);
    ~StdoutLogger() = default;
    void flush() override;
 private:
    void log(const std::string &str, Level log_level) override;
};


class StderrLogger : public BaseLogger {
 public:
    explicit StderrLogger(Level level);
    ~StderrLogger() = default;
    void flush() override;
 private:
    void log(const std::string &str, Level log_level) override;
};

std::shared_ptr<BaseLogger> create_file_logger(Level level, const std::string& filename = "log.txt");

std::shared_ptr<BaseLogger> create_stdout_logger(Level level);

std::shared_ptr<BaseLogger> create_stderr_logger(Level level);
}  // namespace logger

#endif  // INCLUDE_LOGGER_H_
