#ifndef PROJECT_FILELOGGER_H
#define PROJECT_FILELOGGER_H

#include "BaseLogger.h"
#include "fstream"

namespace logger {
class FileLogger : public BaseLogger {
 public:
    FileLogger(Level level, const std::string &filename);
    ~FileLogger() override;
    void flush() override;
 private:
    void log(const std::string &str, Level log_level) override;
    std::ofstream _log_file;
};
}

#endif //PROJECT_FILELOGGER_H
