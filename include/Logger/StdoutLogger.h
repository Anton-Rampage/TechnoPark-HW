#ifndef INCLUDE_LOGGER_STDOUTLOGGER_H_
#define INCLUDE_LOGGER_STDOUTLOGGER_H_

#include "BaseLogger.h"

namespace logger {
class StdoutLogger : public BaseLogger {
 public:
    explicit StdoutLogger(Level level);
    void flush() override;
 private:
    void log(const std::string &str, Level log_level) override;
};
}  // namespace logger

#endif  // INCLUDE_LOGGER_STDOUTLOGGER_H_
