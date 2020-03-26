#include <Logger.h>
#include <iostream>


int main(int argc, char *argv[]) {
    try {
        auto& LOG = logger::Logger::get_instance();
        auto stdout_log = logger::create_stdout_logger(logger::WARN);
        LOG.set_global_logger(stdout_log);

        logger::debug("check debug");
        logger::info("check info");
        logger::warn("check warn");
        logger::error("check error");

        auto stderr_log = logger::create_stderr_logger(logger::ERROR);
        LOG.set_global_logger(stderr_log);

        logger::debug("check debug");
        logger::info("check info");
        logger::warn("check warn");
        logger::error("check error");

        auto file_log = logger::create_file_logger(logger::DEBUG);
        LOG.set_global_logger(file_log);

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

