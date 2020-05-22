#include "Server.h"

#include "BaseException.h"
#include "Connection.h"
#include "Coroutine.h"

#include <csignal>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>

namespace tp {
namespace http {
//static std::atomic<bool> opened = true;

Server::Server(size_t number_of_thread) {
    if (number_of_thread <= 0) {
        throw BaseException("wrong number of threads");
    }
    _thread_pool.reserve(8);
    _number_of_threads = number_of_thread;
    volatile static std::sig_atomic_t opened = 1;
}

Server::~Server() = default;

void Server::epoll_action(int epoll_fd, uint32_t cor, int fd, uint32_t events, int action) {
    epoll_event event{};
    event.data.u32 = cor;
    event.events = events;

    if (epoll_ctl(epoll_fd, action, fd, &event) < 0) {
        throw BaseException("epoll_ctl failed");
    }
}

void Server::run() {
    Server::opened = 1;
    std::signal(SIGTERM, Server::signal_handler);
    std::signal(SIGINT, Server::signal_handler);
    for (size_t i = 0; i < _number_of_threads; ++i) {
        _thread_pool.emplace_back(&Server::loop, this);
    }
}

void Server::loop() {
    Descriptor epoll_fd(epoll_create(1));
    if (epoll_fd < 0) {
        throw BaseException("epoll not created");
    }
    Descriptor server_fd = std::move(create_server());

    epoll_action(epoll_fd.get(), coroutine::current(), server_fd.get(), EPOLLIN | EPOLLEXCLUSIVE, EPOLL_CTL_ADD);
    constexpr size_t epoll_size = 16;
    std::vector<epoll_event> events(epoll_size);
    while (Server::is_opened() == 1) {
        int nfds = epoll_wait(epoll_fd.get(), events.data(), epoll_size, 50);
        if (nfds < 0) {
            if (errno == EINTR) {
                continue;
            }
            throw BaseException("epoll_wait failed");
        }

        for (size_t i = 0; i < nfds; ++i) {
            int cor = events[i].data.u32;
            uint32_t ev = events[i].events;
            try {
                if (cor == coroutine::current()) {
                    coroutine::create_and_run(&Server::connection_routine, this, epoll_fd.get(), server_fd.get());
                } else {
                    if (ev & EPOLLERR || ev & EPOLLHUP) {
                        std::cout << "error" <<std::endl;
                        continue;
                    }
                    coroutine::resume(cor);
                }
            } catch (tp::BaseException& e) { std::cout << e.what() << std::endl; }
        }
    }
}

void Server::connection_routine(int epoll_fd, int server_fd) {
    // init connection
    sockaddr_in client_addr{};
    socklen_t addr_size = sizeof(client_addr);
    Descriptor fd(::accept4(server_fd, reinterpret_cast<sockaddr*>(&client_addr), &addr_size, SOCK_NONBLOCK));
    if (fd < 0) {
        if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
            return;
        }
        throw BaseException("accept4 failed");
    }
    epoll_action(epoll_fd, coroutine::current(), fd.get(), EPOLLIN | EPOLLET | EPOLLONESHOT, EPOLL_CTL_ADD);

    int buf_fd = fd.get();
    Connection connection(std::move(fd));

    while (true) {
        coroutine::yield();
        // read
        std::string request;
        while (true) {
            char read_buffer[1024]{};
            connection.read(read_buffer, sizeof(read_buffer));
            request += read_buffer;
            if (!connection.is_readable()) {
                epoll_action(epoll_fd, coroutine::current(), buf_fd, EPOLLOUT | EPOLLET | EPOLLONESHOT, EPOLL_CTL_DEL);
                return;
            }
            if (errno == EAGAIN) {
                break;
            }
        }

        epoll_action(epoll_fd, coroutine::current(), buf_fd, EPOLLOUT | EPOLLET | EPOLLONESHOT, EPOLL_CTL_MOD);
        coroutine::yield();
        HttpRequest parse_request(request);
        parse_request.parse();
        HttpResponse http_response = on_request(parse_request);
        const char* response = http_response.get();
        // write

        size_t write_size = strlen(response);
        while (true) {
            size_t len = connection.write(response, write_size);
            if (len < write_size) {
                write_size -= len;
                response += len;
                epoll_action(epoll_fd, coroutine::current(), buf_fd, EPOLLOUT | EPOLLET | EPOLLONESHOT, EPOLL_CTL_MOD);
                coroutine::yield();
            } else {
                break;
            }
        }

        if (parse_request.is_continue) {
            epoll_action(epoll_fd, coroutine::current(), buf_fd, EPOLLIN | EPOLLET | EPOLLONESHOT, EPOLL_CTL_MOD);
            continue;
        }
        break;
    }
    epoll_action(epoll_fd, coroutine::current(), buf_fd, EPOLLOUT | EPOLLET | EPOLLONESHOT, EPOLL_CTL_DEL);

}

void Server::stop() {
//    opened = false;
    for (auto& thread : _thread_pool) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

Descriptor Server::create_server() {
    Descriptor server(socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0));
    if (server < 0) {
        throw BaseException("socket not created");
    }

    int enable = 1;
    if (setsockopt(server.get(), SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0) {
        throw BaseException("setsockopt failed");
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server.get(), reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        throw BaseException("bind failed");
    }

    constexpr size_t MAX_CONNECTIONS = 256;
    if (listen(server.get(), MAX_CONNECTIONS) < 0) {
        throw BaseException("listen failed");
    }
    return std::move(server);
}
HttpResponse Server::on_request(const HttpRequest& req) {
    return HttpResponse("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nConnection: Keep-Alive\r\n\r\n");
}
auto& Server::is_opened() {
    volatile static std::sig_atomic_t opened = 1;
    return opened;
}

auto&& Server::is_opened() {
    volatile static std::sig_atomic_t opened = 1;
    return opened;
}
//bool Server::is_opened() { return opened; }

}  // namespace http
}  // namespace tp
