#ifndef INCLUDE_TCP_SERVER_H_
#define INCLUDE_TCP_SERVER_H_

#include <memory>
#include "Connection.h"

namespace tcp {
using ConnectionPtr = std::unique_ptr<Connection>;
class Server {
 public:
    Server(const std::string& ip, int port);
    ~Server();

    void open(const std::string& ip, int port);
    void close();

    bool is_opened();
    void set_max_connect(int num);

    ConnectionPtr accept();
 private:
    process::Descriptor _fd;
    bool _opened;
};
}  // namespace tcp

#endif  // INCLUDE_TCP_SERVER_H_
