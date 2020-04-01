#include "Connection.h"
#include "Logger.h"

int main() {
    logger::Logger& LOG = logger::Logger::get_instance();
    logger::BaseLoggerPtr stdout_log = logger::create_stdout_logger(logger::Level::DEBUG);
    LOG.set_global_logger(std::move(stdout_log));
    try {
        tcp::Connection con_1 = {"127.0.0.1", 9625};
        tcp::Connection con_2 = {"127.0.0.1", 9625};
        logger::info("first and second connection to");
        logger::info("dst_ip: " + con_1.get_dst_ip());
        logger::info("dst_port: " + std::to_string(con_1.get_dst_port()));

        std::string write_data = "hello";
        con_1.write(write_data.c_str(), write_data.size());
        con_2.write(write_data.c_str(), write_data.size());

        char read_data[256]{};
        con_1.read(read_data, sizeof(read_data));
        logger::debug(std::string("from 1: ") + read_data);

        con_2.read(read_data, sizeof(read_data));
        logger::debug(std::string("from 2: ") + read_data);
    }
    catch (tcp::TcpException& e) {
        logger::error(e.what());
    }
}


