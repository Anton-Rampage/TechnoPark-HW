#include "Logger.h"
#include <iostream>
#include "LogSettings.h"

int main(int argc, char *argv[]) {
    LogSettings settings{};
    int ret_value = set_settings(argc, argv, settings);
    if (ret_value != 0) {
        return 0;
    }
    try {
        logger::Logger& LOG = logger::Logger::get_instance();
        if (settings.get_stream() == LogStream::STDOUT) {
            logger::BaseLoggerPtr stdout_log = logger::create_stdout_logger(settings.get_level());
            LOG.set_global_logger(std::move(stdout_log));
        } else if (settings.get_stream() == LogStream::STDERR) {
            logger::BaseLoggerPtr stderr_log = logger::create_stderr_logger(settings.get_level());
            LOG.set_global_logger(std::move(stderr_log));
        } else if (settings.get_stream() == LogStream::FILE) {
            logger::BaseLoggerPtr file_log = logger::create_file_logger(settings.get_level(),
                                                                                      settings.get_path());
            LOG.set_global_logger(std::move(file_log));
        }
        logger::debug("check debug");
        logger::info("check info");
        logger::warn("check warn");
        logger::error("check error");
    }
    catch (logger::LogException& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
