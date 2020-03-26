#include "LogSettings.h"
#include <getopt.h>
#include <vector>
#include <iostream>

LogSettings::LogSettings(logger::Level level,
                         LogStream stream,
                         std::string file_path)
    : _level(level)
    , _log_stream(stream)
    , _file_path(std::move(file_path)) {}

void LogSettings::set_level(logger::Level level) {
    _level = level;
}

void LogSettings::set_stream(LogStream stream) {
    _log_stream = stream;
}

void LogSettings::set_path(std::string path) {
    _file_path = std::move(path);
}

logger::Level LogSettings::get_level() const {
    return _level;
}

LogStream LogSettings::get_stream() const {
    return _log_stream;
}

std::string LogSettings::get_path() const {
    return _file_path;
}

size_t get_level_from_string(const char *str) {
    std::vector<std::string> levels = {"error", "warn", "info", "debug"};
    for (size_t i = 0; i < levels.size(); ++i) {
        if (levels[i] == str) {
            return i + 1;
        }
    }
    return 0;
}
void display_usage() {
    std::cout << "-l or --log-level set log level with args : debug, info, warn, error" << std::endl
              << "--log-to-file and path to file output log to file" << std::endl
              << "--log-to-stdout output log to stdout" << std::endl
              << "--log-to-stderr output log to stderr" << std::endl
              << "if no opts use default settings is stdout and warn" << std::endl;
}

int set_settings(int argc, char **argv, LogSettings& settings) {
    if (argc == 1) {
        return 0;
    }

    struct option opts[] = {
            {"log-level",     1, nullptr, 'l'},
            {"log-to-file",   1, nullptr, 0},
            {"log-to-stdout", 0, nullptr, 0},
            {"log-to-stderr", 0, nullptr, 0},
            {"help",          0, nullptr, 'h'},
            {nullptr,         0, nullptr, 0}
    };

    const char *opt_string = "l:h";
    int opt_index = 0;

    int opt = getopt_long(argc, argv, opt_string, opts, &opt_index);

    while (opt != -1) {
        if (opt == 'l') {
            size_t level = get_level_from_string(optarg);
            if (level == 0) {
                std::cout << "error params" << std::endl;
                return 1;
            }
            settings.set_level(static_cast<logger::Level>(level));
        } else if (opt == 'h') {
            display_usage();
            return 2;
        } else if (opt == 0) {
            if (std::string("log-to-file") == opts[opt_index].name) {
                settings.set_stream(LogStream::FILE);
                settings.set_path(optarg);
            } else if (std::string("log-to-stdout") == opts[opt_index].name) {
                settings.set_stream(LogStream::STDOUT);
            } else if (std::string("log-to-stderr") == opts[opt_index].name) {
                settings.set_stream(LogStream::STDERR);
            }
        } else {
            return 1;
        }
        opt = getopt_long(argc, argv, opt_string, opts, &opt_index);
    }

    return 0;
}




