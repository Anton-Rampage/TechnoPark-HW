#include <StderrLogger.h>
#include "EpollServer.h"
#include "Logger.h"

int main() {
    logger::Logger& LOG = logger::Logger::get_instance();
    char read[] = "Hello! By!";
    tcp::callback create_handler = [](tcp::Connection& con) {
        char write_data[] = "Hello! By!";
        con.set_cache_size(sizeof(read), sizeof(write_data), write_data);
        logger::info("new connection");
        logger::info("dst_ip: " + con.get_dst_ip());
        logger::info("dst_port: " + std::to_string(con.get_dst_port()));
    };

    tcp::callback read_handler = [](tcp::Connection& con) {
        char *read_data = new char[con.get_cache_size_read()];
        size_t len = con.read(read_data, sizeof(con.get_cache_size_read()));
        con.add_cache_read(read_data, len);
        logger::debug(std::string("read: ") + read_data);
        delete[] read_data;
    };

    tcp::callback write_handler = [](tcp::Connection& con) {
        size_t len = con.write(con.get_cache_write(), con.get_cache_size_write());
        con.del_cache_write(len);
        logger::debug(std::string("send: ") + std::to_string(len));
    };

    tcp::Handlers handlers = {create_handler, read_handler, write_handler};
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

