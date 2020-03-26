#ifndef PROJECT_STDOUTLOGGER_H
#define PROJECT_STDOUTLOGGER_H

#include "BaseLogger.h"

namespace logger {
class StdoutLogger : public BaseLogger {
 public:
    explicit StdoutLogger(Level level);
    ~StdoutLogger() override = default;
    void flush() override;
 private:
    void log(const std::string &str, Level log_level) override;
};
}

#endif //PROJECT_STDOUTLOGGER_H
