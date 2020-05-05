#ifndef INCLUDE_LOGGER_FILELOGGER_H_
#define INCLUDE_LOGGER_FILELOGGER_H_

#include "BaseLogger.h"

#include <fstream>

namespace tp {
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
}  // namespace logger
}  // namespace tp

#endif  // INCLUDE_LOGGER_FILELOGGER_H_
