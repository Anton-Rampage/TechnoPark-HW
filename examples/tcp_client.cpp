#include "Connection.h"
#include "Logger.h"

#include <BaseException.h>
#include <StderrLogger.h>
#include <thread>

using tp::logger::debug;
using tp::logger::info;

int main() {
    auto& LOG = tp::logger::Logger::get_instance();
    try {
        tp::logger::BaseLoggerPtr stdout_log = tp::logger::create_stdout_logger(tp::logger::Level::DEBUG);
        LOG.set_global_logger(std::move(stdout_log));
        tp::tcp::Connection con_1 = {"127.0.0.1", 8080};

        info("connection to:");
        info("dst_ip: " + con_1.get_dst_ip());
        info("dst_port: " + std::to_string(con_1.get_dst_port()));

        std::string write_data =
            "GET /wiki/страница HTTP/1.1\r\n"
            "Host: ru.wikipedia.org\r\n"
            "User-Agent: Mozilla/5.0; Gecko/2008050509; Firefox/3.0b5\r\n"
            "Accept: text/html\r\n"
            "Connection: Keep-Alive\r\n\r\n";
        debug("send 1: " + std::to_string(con_1.write(write_data.c_str(), write_data.size() + 1)));

        char read_data[1024]{};
        con_1.read(read_data, sizeof(read_data));
        debug(std::string("from 1: ") + read_data);

        std::this_thread::sleep_for(std::chrono::seconds(10));

        debug("send 1: " + std::to_string(con_1.write(write_data.c_str(), write_data.size() + 1)));

        con_1.read(read_data, sizeof(read_data));
        debug(std::string("from 1: ") + read_data);
    } catch (tp::BaseException& e) { tp::logger::StderrLogger{tp::logger::Level::ERROR}.error(e.what()); }
}
