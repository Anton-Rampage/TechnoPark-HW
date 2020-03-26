#include "FileLogger.h"

namespace logger {
FileLogger::FileLogger(Level level, const std::string &filename)
        : BaseLogger(level),
          _log_file(filename) {
    if (!_log_file.is_open()) {
        throw LogException("file not opened");
    }
}

void FileLogger::log(const std::string &str, Level log_level) {
    if (log_level <= level()) {
        _log_file << str << std::endl;
    }
}

void FileLogger::flush() {
    _log_file << std::flush;
}

FileLogger::~FileLogger() {
    _log_file.close();
}
}