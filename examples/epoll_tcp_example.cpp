#include <StderrLogger.h>
#include "EpollServer.h"
#include "Logger.h"

int main() {
    logger::Logger& LOG = logger::Logger::get_instance();

    tcp::callback read_handler = [](tcp::Connection& con) {
        logger::info("new connection");
        logger::info("dst_ip: " + con.get_dst_ip());
        logger::info("dst_port: " + std::to_string(con.get_dst_port()));
        char read_data[256]{};
        con.read(read_data, sizeof(read_data));
        logger::debug(std::string("read: ") + read_data);
    };

    tcp::callback write_handler = [](tcp::Connection& con) {
        std::string write_data = "received";
        size_t len = con.write(write_data.c_str(), write_data.size());
        logger::debug(std::string("send: ") + std::to_string(len));
    };


    tcp::read_write_cb handlers = {read_handler, write_handler};
    try {
        LOG.set_global_logger(create_stdout_logger(logger::Level::DEBUG));
        tcp::EpollServer serv(9625, handlers);
        serv.event_loop(10);
    }
    catch (tcp::TcpException& e) {
        logger::StderrLogger{logger::Level::ERROR}.error(e.what());
    }
    return 0;
}

