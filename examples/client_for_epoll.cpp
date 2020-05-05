#include "Connection.h"
#include "Logger.h"

#include <BaseException.h>
#include <StderrLogger.h>

using tp::logger::debug;
using tp::logger::info;

int main() {
    auto& LOG = tp::logger::Logger::get_instance();
    try {
        tp::logger::BaseLoggerPtr stdout_log = tp::logger::create_stdout_logger(tp::logger::Level::DEBUG);
        LOG.set_global_logger(std::move(stdout_log));
        tp::tcp::Connection con_1 = {"127.0.0.1", 9625};

        info("connection to:");
        info("dst_ip: " + con_1.get_dst_ip());
        info("dst_port: " + std::to_string(con_1.get_dst_port()));

        std::string write_data = "Hello!";
        debug("send 1: " + std::to_string(con_1.write(write_data.c_str(), write_data.size() + 1)));

        std::string read_data;
        read_data.resize(6);
        con_1.read(read_data.data(), read_data.size());
        debug(std::string("from 1: ") + read_data);

        write_data = "By!";
        debug("send 1: " + std::to_string(con_1.write(write_data.c_str(), write_data.size())));

        read_data.clear();
        read_data.resize(4);
        con_1.read(read_data.data(), read_data.size());
        debug(std::string("from 1: ") + read_data);
    } catch (tp::BaseException& e) { tp::logger::StderrLogger{tp::logger::Level::ERROR}.error(e.what()); }
}