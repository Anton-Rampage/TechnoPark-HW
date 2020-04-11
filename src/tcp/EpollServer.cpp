#include "EpollServer.h"
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>

namespace tcp {

EpollServer::EpollServer(uint32_t port, read_write_cb& handler) {
    process::Descriptor epoll(epoll_create(1));
    if (epoll < 0) {
        throw TcpException("epoll not created");
    }
    _epoll_fd = epoll.extract();
    open(port, handler);
}

void EpollServer::open(uint32_t port, read_write_cb& handler) {
    process::Descriptor serv(socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0));
    if (serv < 0) {
        throw TcpException("socket not created");
    }
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serv.get(), reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
        throw TcpException("bind failed");
    }

    constexpr size_t QUEUE_SIZE = 256;
    if (listen(serv.get(), QUEUE_SIZE) < 0) {
        throw TcpException("listen failed");
    }

    epoll_action(serv.get(), EPOLLIN, EPOLL_CTL_ADD);

    _server_fd = serv.extract();
    _opened = true;
    _handler = handler;
}

void EpollServer::epoll_action(int fd, uint32_t events, int action) {
    epoll_event event{};
    event.data.fd = fd;
    event.events = events;

    if (epoll_ctl(_epoll_fd.get(), action, fd, &event) < 0) {
        throw TcpException("epoll ctl add failed");
    }
}

EpollServer::~EpollServer() {
    close();
    _epoll_fd.close();
}

void EpollServer::close() {
    epoll_action(_server_fd.get(), EPOLLIN, EPOLL_CTL_DEL);
    _server_fd.close();
}

void EpollServer::event_loop(size_t epoll_size) {
    epoll_event events[25];
    while (true) {
        int nfds = epoll_wait(_epoll_fd.get(), events, epoll_size, -1);
        if (nfds < 0) {
            if (errno == EINTR) {
                continue;
            }
            throw TcpException("epoll_wait failed");
        }

        for(size_t i = 0; i < nfds; ++i) {
            int fd = events[i].data.fd;
            auto ev = events[i].events;
            if (fd == _server_fd.get()) {
                accept();
            } else {
                if (ev & EPOLLIN) {
                    _handler[0](connections[fd]);

                    if (connections[fd].is_writable()) {
                        epoll_action(fd, EPOLLOUT, EPOLL_CTL_MOD);
                    } else if (!connections[fd].is_readable() && !connections[fd].is_writable()){
                        epoll_action(fd, EPOLLIN, EPOLL_CTL_DEL);
                        connections.erase(fd);
                    }
                } else if (ev & EPOLLOUT) {
                    _handler[1](connections[fd]);

                    if (connections[fd].is_readable()) {
                        epoll_action(fd, EPOLLIN, EPOLL_CTL_MOD);
                    } else if (!connections[fd].is_readable() && !connections[fd].is_writable()){
                        epoll_action(fd, EPOLLOUT, EPOLL_CTL_DEL);
                        connections.erase(fd);
                    }
                }
            }
        }
    }
}

void EpollServer::accept() {
    sockaddr_in client_addr{};
    socklen_t addr_size = sizeof(client_addr);
    process::Descriptor fd;
    while (true) {
        fd = ::accept4( _server_fd.get(),
                       reinterpret_cast<sockaddr *>(&client_addr),
                       &addr_size,
                       SOCK_NONBLOCK);
        if (fd < 0) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return;
            }
            throw TcpException("accept4 failed");
        }
        epoll_action(fd.get(), EPOLLIN, EPOLL_CTL_ADD);

        int buf_fd = fd.get();
        Connection connection(std::move(fd));
        connections.insert(std::pair<int, Connection>(buf_fd, std::move(connection)));
        _handler[2](connections[buf_fd]);
    }
}
}  // namespace tcp
