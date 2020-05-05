#ifndef INCLUDE_LOGGER_STDERRLOGGER_H_
#define INCLUDE_LOGGER_STDERRLOGGER_H_

#include "BaseLogger.h"

namespace tp {
namespace logger {
class StderrLogger : public BaseLogger {
 public:
    explicit StderrLogger(Level level);
    void flush() override;

 private:
    void log(const std::string &str, Level log_level) override;
};
}  // namespace logger
}  // namespace tp

#endif  // INCLUDE_LOGGER_STDERRLOGGER_H_
