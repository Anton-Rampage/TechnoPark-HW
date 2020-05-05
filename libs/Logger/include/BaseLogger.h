#ifndef INCLUDE_LOGGER_BASELOGGER_H_
#define INCLUDE_LOGGER_BASELOGGER_H_

#include <array>
#include <exception>
#include <string>

namespace tp {
namespace logger {
enum class Level : int { DEBUG = 4, INFO = 3, WARN = 2, ERROR = 1 };

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

 protected:
    std::array<std::string, 4> level_str = {"[ERROR]", "[WARN]", "[INFO]", "[DEBUG]"};

 private:
    virtual void log(const std::string &str, Level level) = 0;

    Level _level;
};
}  // namespace logger
}  // namespace tp

#endif  // INCLUDE_LOGGER_BASELOGGER_H_
