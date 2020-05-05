#ifndef INCLUDE_TCP_EPOLLSERVER_H_
#define INCLUDE_TCP_EPOLLSERVER_H_

#include "Descriptor.h"
#include "EpollConnection.h"

#include <functional>
#include <map>

namespace tp {
namespace epolltcp {
using callback = std::function<void(EpollConnection&)>;
struct Handlers {
    callback create;
    callback read;
    callback write;
};

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
    Descriptor _server_fd;
    Descriptor _epoll_fd;
    bool _opened = false;
    Handlers _handlers;
    std::map<int, EpollConnection> _connections;
};

}  // namespace epolltcp
}  // namespace tp

#endif  // INCLUDE_TCP_EPOLLSERVER_H_
