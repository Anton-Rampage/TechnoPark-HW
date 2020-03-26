#ifndef PROJECT_STDERRLOGGER_H
#define PROJECT_STDERRLOGGER_H

#include "BaseLogger.h"

namespace logger {
class StderrLogger : public BaseLogger {
 public:
    explicit StderrLogger(Level level);
    ~StderrLogger() override = default;
    void flush() override;
 private:
    void log(const std::string &str, Level log_level) override;
};
}

#endif //PROJECT_STDERRLOGGER_H
