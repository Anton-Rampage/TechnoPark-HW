#include "EpollServer.h"
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>

namespace tcp {

EpollServer::EpollServer(uint32_t port, Handlers& handlers) {
    _epoll_fd = process::Descriptor(epoll_create(1));
    if (_epoll_fd < 0) {
        throw TcpException("epoll not created");
    }
    open(port, handlers);
}

void EpollServer::open(uint32_t port, Handlers& handlers) {
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

    constexpr size_t MAX_CONNECTIONS = 256;
    if (listen(serv.get(), MAX_CONNECTIONS) < 0) {
        throw TcpException("listen failed");
    }

    epoll_action(serv.get(), EPOLLIN, EPOLL_CTL_ADD);

    _server_fd = serv.extract();
    _opened = true;
    _handlers = handlers;
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
    _opened = false;
}

void EpollServer::event_loop(size_t epoll_size) {
    std::vector<epoll_event> events(epoll_size);
    while (true) {
        int nfds = epoll_wait(_epoll_fd.get(), events.data(), epoll_size, -1);
        if (nfds < 0) {
            if (errno == EINTR) {
                continue;
            }
            throw TcpException("epoll_wait failed");
        }

        for(size_t i = 0; i < nfds; ++i) {
            int fd = events[i].data.fd;
            uint32_t ev = events[i].events;
            if (fd == _server_fd.get()) {
                accept();
            } else {
                handle_client(fd, ev);
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
        _connections.insert(std::pair<int, Connection>(buf_fd, std::move(connection)));
        _handlers.create(_connections[buf_fd]);
    }
}

void EpollServer::handle_client(int fd, uint32_t event) {
    if (event & EPOLLERR || event & EPOLLHUP) {
        epoll_action(fd, 0, EPOLL_CTL_DEL);
        _connections.erase(fd);
    }
    if (event & EPOLLIN) {
        _handlers.read(_connections[fd]);

        if (_connections[fd].is_writable()) {
            epoll_action(fd, EPOLLOUT, EPOLL_CTL_MOD);
        }
    } else if (event & EPOLLOUT) {
        _handlers.write(_connections[fd]);

        if (_connections[fd].is_readable()) {
            epoll_action(fd, EPOLLIN, EPOLL_CTL_MOD);
        }
    }
    if (!_connections[fd].is_readable() && !_connections[fd].is_writable()){
        epoll_action(fd, 0, EPOLL_CTL_DEL);
        _connections.erase(fd);
    }
}

void EpollServer::set_max_connections(size_t size) {
    if (listen(_server_fd.get(), size) < 0) {
        throw TcpException("listen failed");
    }
}
}  // namespace tcp
