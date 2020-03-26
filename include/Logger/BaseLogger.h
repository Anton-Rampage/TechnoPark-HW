#ifndef PROJECT_BASELOGGER_H
#define PROJECT_BASELOGGER_H

#include <string>


namespace logger {
enum class Level : int {
    DEBUG = 4,
    INFO = 3,
    WARN = 2,
    ERROR = 1
};

class LogException : public std::exception {
 public:
    explicit LogException(std::string error);
    const char* what() const noexcept override;
 private:
    std::string _error;
};

class BaseLogger {
 public:
    explicit BaseLogger(Level level);

    virtual ~BaseLogger() = default;

    void debug(const std::string &str);

    void info(const std::string &str);

    void warn(const std::string &str);

    void error(const std::string &str);

    void set_level(Level level);

    Level level();

    virtual void flush() = 0;

 private:
    virtual void log(const std::string &str, Level level) = 0;

    Level _level;
};
}

#endif //PROJECT_BASELOGGER_H
