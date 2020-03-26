#ifndef EXAMPLES_LOGSETTINGS_H_
#define EXAMPLES_LOGSETTINGS_H_

#include <Logger.h>

enum class LogStream {
    STDOUT,
    STDERR,
    FILE
};

class LogSettings {
 public:
    explicit LogSettings(logger::Level level = logger::WARN,
                         LogStream stream = LogStream::STDOUT,
                         std::string file_path = "log.txt");

    void set_level(logger::Level level);
    void set_stream(LogStream stream);
    void set_path(std::string path);

    logger::Level get_level() const;
    LogStream get_stream() const;
    std::string get_path() const;
 private:
    logger::Level _level;
    LogStream _log_stream;
    std::string _file_path;
};

size_t get_level_from_string(const char *str);
void display_usage();

int set_settings(int argc, char *argv[], LogSettings& settings);


#endif  // EXAMPLES_LOGSETTINGS_H_
