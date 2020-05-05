#ifndef INCLUDE_TCP_SERVER_H_
#define INCLUDE_TCP_SERVER_H_

#include "Connection.h"

#include <Descriptor.h>

namespace tp {
namespace tcp {
class Server {
 public:
    Server(const std::string& ip, int port);
    ~Server();

    void open(const std::string& ip, int port);
    void close();

    bool is_opened();
    void set_max_connect(int num);

    Connection accept();

 private:
    Descriptor _fd;
    bool _opened;
};
}  // namespace tcp
}  // namespace tp

#endif  // INCLUDE_TCP_SERVER_H_
