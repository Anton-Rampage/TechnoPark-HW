#include "FileLogger.h"

#include "BaseException.h"

#include <chrono>
#include <iomanip>

namespace tp {
namespace logger {
FileLogger::FileLogger(Level level, const std::string &filename) : BaseLogger(level), _log_file(filename) {
    if (!_log_file.is_open()) {
        throw BaseException("file not opened");
    }
}

void FileLogger::log(const std::string &str, Level log_level) {
    if (log_level <= level()) {
        auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        _log_file << "[" << std::put_time(std::localtime(&time), "%X") << "] "
                  << level_str[static_cast<int>(log_level) - 1] << ' ' << str << std::endl;
    }
}

void FileLogger::flush() { _log_file << std::flush; }

FileLogger::~FileLogger() { _log_file.close(); }
}  // namespace logger
}  // namespace tp
