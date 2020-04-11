#ifndef INCLUDE_TCP_EPOLLSERVER_H_
#define INCLUDE_TCP_EPOLLSERVER_H_

#include <functional>
#include <map>
#include "Connection.h"
#include "Descriptor.h"

namespace tcp {
using callback = std::function<void(Connection&)>;
using read_write_cb = std::array<callback, 3>;
class EpollServer {
 public:
    explicit EpollServer(uint32_t port, read_write_cb& handler);
    void open(uint32_t port, read_write_cb& handler);
    ~EpollServer();
    void close();
    void event_loop(size_t epoll_size);

 private:
    void accept();
    void epoll_action(int fd, uint32_t events, int action);
    process::Descriptor _server_fd;
    process::Descriptor _epoll_fd;
    bool _opened;
    read_write_cb _handler;
    std::map<int, Connection> connections;
};

}  // namespace tcp

#endif  // INCLUDE_TCP_EPOLLSERVER_H_
