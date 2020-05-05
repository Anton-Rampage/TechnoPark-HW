#include "EpollServer.h"
#include "Logger.h"

#include <BaseException.h>
#include <StderrLogger.h>

using tp::logger::debug;
using tp::logger::info;

int main() {
    tp::logger::Logger& LOG = tp::logger::Logger::get_instance();
    tp::epolltcp::callback create_handler = [](tp::epolltcp::EpollConnection& con) {
        std::string write_data = "Hello! By!";
        con.set_cache_size(20, write_data.size(), write_data.data());
        info("new connection");
        info("dst_ip: " + con.get_dst_ip());
        info("dst_port: " + std::to_string(con.get_dst_port()));
    };

    tp::epolltcp::callback read_handler = [](tp::epolltcp::EpollConnection& con) {
        std::string read_data;
        read_data.resize(con.get_cache_size_read());
        size_t len = con.read(read_data.data(), con.get_cache_size_read());
        if (len != 0) {
            debug(std::string("read: ") + con.get_cache_read());
        }
    };

    tp::epolltcp::callback write_handler = [](tp::epolltcp::EpollConnection& con) {
        size_t len = con.write(con.get_cache_write(), con.get_cache_size_write());
        debug(std::string("send: ") + std::to_string(len));
    };

    tp::epolltcp::Handlers handlers = {create_handler, read_handler, write_handler};
    try {
        LOG.set_global_logger(create_stdout_logger(tp::logger::Level::DEBUG));
        tp::epolltcp::EpollServer serv(9625, handlers);
        serv.event_loop(10);
    } catch (tp::BaseException& e) { tp::logger::StderrLogger{tp::logger::Level::ERROR}.error(e.what()); }
    return 0;
}
