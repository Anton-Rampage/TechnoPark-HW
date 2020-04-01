#include "Server.h"
#include "Logger.h"

int main() {
    logger::Logger& LOG = logger::Logger::get_instance();
    logger::BaseLoggerPtr stdout_log = logger::create_stdout_logger(logger::Level::DEBUG);
    LOG.set_global_logger(std::move(stdout_log));
    try {
        tcp::Server serv = {"127.0.0.1", 9625};
        while (true) {
            tcp::ConnectionPtr connect = serv.accept();
            logger::info("connect");
            logger::info("dst_ip: " + connect->get_dst_ip());
            logger::info("dst_port: " + std::to_string(connect->get_dst_port()));

            char read_data[256]{};
            connect->read(read_data, sizeof(read_data));
            logger::debug(std::string("read: ") + read_data);

            std::string write_data = "received";
            connect->write(write_data.c_str(), write_data.size());
        }
    }
    catch (tcp::TcpException& e) {
        logger::error(e.what());
    }
}
