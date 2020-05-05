#include "BaseException.h"
#include "LogSettings.h"
#include "Logger.h"
#include "StderrLogger.h"

int main(int argc, char* argv[]) {
    LogSettings settings{};
    int ret_value = set_settings(argc, argv, settings);
    if (ret_value != 0) {
        return 0;
    }
    tp::logger::Logger& LOG = tp::logger::Logger::get_instance();
    try {
        if (settings.get_stream() == LogStream::STDOUT) {
            LOG.set_global_logger(std::move(tp::logger::create_stdout_logger(settings.get_level())));
        } else if (settings.get_stream() == LogStream::STDERR) {
            LOG.set_global_logger(tp::logger::create_stderr_logger(settings.get_level()));
        } else if (settings.get_stream() == LogStream::FILE) {
            LOG.set_global_logger(tp::logger::create_file_logger(settings.get_level(), settings.get_path()));
        }
        tp::logger::debug("check debug");
        tp::logger::info("check info");
        tp::logger::warn("check warn");
        tp::logger::error("check error");
    } catch (tp::BaseException& e) { tp::logger::StderrLogger{tp::logger::Level::ERROR}.error(e.what()); }
    return 0;
}
