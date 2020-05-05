#ifndef EXAMPLES_LOGSETTINGS_H_
#define EXAMPLES_LOGSETTINGS_H_

#include "Logger.h"

enum class LogStream { STDOUT, STDERR, FILE };

class LogSettings {
 public:
    explicit LogSettings(tp::logger::Level level = tp::logger::Level::WARN, LogStream stream = LogStream::STDOUT,
                         std::string file_path = "log.txt");

    void set_level(tp::logger::Level level);
    void set_stream(LogStream stream);
    void set_path(std::string path);

    [[nodiscard]] tp::logger::Level get_level() const;
    [[nodiscard]] LogStream get_stream() const;
    [[nodiscard]] std::string get_path() const;

 private:
    tp::logger::Level _level;
    LogStream _log_stream;
    std::string _file_path;
};

size_t get_level_from_string(const char *str);
void display_usage();

int set_settings(int argc, char *argv[], LogSettings &settings);

#endif  // EXAMPLES_LOGSETTINGS_H_
