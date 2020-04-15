#ifndef INCLUDE_TCP_EPOLLSERVER_H_
#define INCLUDE_TCP_EPOLLSERVER_H_

#include <functional>
#include <map>
#include "Connection.h"
#include "Descriptor.h"

namespace tcp {
using callback = std::function<void(Connection&)>;
class Handlers {
 public:
    callback create;
    callback read;
    callback write;
};

using read_write_cb = std::array<callback, 3>;
class EpollServer {
 public:
    explicit EpollServer(uint32_t port, Handlers& handlers);
    void open(uint32_t port, Handlers& handlers);
    ~EpollServer();
    void close();
    void event_loop(size_t epoll_size);
    void set_max_connections(size_t size);

 private:
    void accept();
    void handle_client(int fd, uint32_t event);
    void epoll_action(int fd, uint32_t events, int action);
    process::Descriptor _server_fd;
    process::Descriptor _epoll_fd;
    bool _opened = false;
    Handlers _handlers;
    std::map<int, Connection> _connections;
};

}  // namespace tcp

#endif  // INCLUDE_TCP_EPOLLSERVER_H_
