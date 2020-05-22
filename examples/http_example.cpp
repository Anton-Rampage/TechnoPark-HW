#include "BaseException.h"
#include "Logger.h"
#include "Server.h"

#include <StderrLogger.h>
#include <csignal>
using tp::logger::info;


bool open = true;


int main() {
    try {

        auto& LOG = tp::logger::Logger::get_instance();
        LOG.set_global_logger(tp::logger::create_stdout_logger(tp::logger::Level::DEBUG));
        tp::http::Server server(4);

        server.run();
        server.stop();

    } catch (tp::BaseException& e) { tp::logger::StderrLogger{tp::logger::Level::ERROR}.error(e.what()); }
    return 0;
}