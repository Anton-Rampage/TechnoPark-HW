#include "Logger.h"
#include "Server.h"

#include <BaseException.h>
#include <StderrLogger.h>

using tp::logger::debug;
using tp::logger::info;

int main() {
    try {
        auto& LOG = tp::logger::Logger::get_instance();
        LOG.set_global_logger(create_stdout_logger(tp::logger::Level::DEBUG));
        tp::tcp::Server serv{"127.0.0.1", 9625};
        while (true) {
            tp::tcp::Connection connect = serv.accept();
            info("new connection");
            info("dst_ip: " + connect.get_dst_ip());
            info("dst_port: " + std::to_string(connect.get_dst_port()));

            char read_data[7]{};
            connect.read_exact(read_data, sizeof(read_data));
            debug(std::string("read: ") + read_data);

            std::string write_data = "received";
            connect.write_exact(write_data.c_str(), write_data.size());
        }
    } catch (tp::BaseException& e) { tp::logger::StderrLogger{tp::logger::Level::ERROR}.error(e.what()); }
}
