#include "Server.h"

#include "BaseException.h"
#include "Connection.h"
#include "Coroutine.h"
#include "Epoll.h"
#include "Logger.h"

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>


namespace tp {
using tp::logger::debug;
using tp::logger::error;
namespace http {

Server::Server(size_t number_of_thread) : logger(logger::Logger::get_instance()) {
    if (number_of_thread <= 0) {
        throw BaseException("wrong number of threads");
    }
    _thread_pool.reserve(number_of_thread);
    _number_of_threads = number_of_thread;
    logger.set_global_logger(tp::logger::create_stdout_logger(tp::logger::Level::DEBUG));
}

Server::~Server() = default;

void Server::run() {
    for (size_t i = 0; i < _number_of_threads; ++i) {
        _thread_pool.emplace_back(&Server::loop, this);
    }
}

void Server::loop() {
    constexpr size_t epoll_size = 100;
    Epoll epoll(epoll_size);
    Descriptor server_fd = std::move(create_server());

    auto cur_coroutine = coroutine::current();
    epoll.add(cur_coroutine, server_fd.get(), EPOLLIN);

    while (true) {
        int nfds = epoll.wait();

        if (nfds < 0) {
            if (errno == EINTR) {
                continue;
            }
            throw BaseException("epoll_wait failed");
        }
        if (nfds > 0) {
            debug(std::to_string(nfds));
        }

        for (size_t i = 0; i < nfds; ++i) {

            int cor = epoll.epoll_events[i].data.u32;
            uint32_t ev = epoll.epoll_events[i].events;
            try {
                if (cor == cur_coroutine) {
                    coroutine::create_and_run(&Server::connection_routine, this, std::ref(epoll), server_fd.get());
                } else {
                    if (ev & EPOLLERR || ev & EPOLLHUP) {
                        std::cout << "error" << std::endl;
                        continue;
                    }
                    coroutine::resume(cor);
                }
            } catch (tp::BaseException& e) { error(e.what()); }
        }
    }
}

void Server::connection_routine(Epoll& epoll, int server_fd) {
    // init connection
    Connection connection = accept(server_fd);
    if (!connection.is_readable()) {
        return;
    }

    auto cur_coroutine = coroutine::current();
    int buf_fd = connection._fd.get();
    epoll.add(cur_coroutine, buf_fd, EPOLLIN);
    std::string response;

    while (true) {
        // read
        coroutine::yield();
        std::string request = std::move(connection.read_exact());
        if (!connection.is_readable()) {
            epoll.del(cur_coroutine, buf_fd, EPOLLOUT);
            break;
        }
        epoll.mod(cur_coroutine, buf_fd, EPOLLOUT);
        debug("read" + request);

        // write
        coroutine::yield();
        HttpRequest parse_request(request);
        parse_request.parse();
        HttpResponse http_response = on_request(parse_request);
        response = http_response.get();
        connection.write_exact(response.data(), response.size());

        if (parse_request.is_continue) {
            epoll.mod(cur_coroutine, buf_fd, EPOLLIN);
            continue;
        }
        break;
    }
    epoll.del(cur_coroutine, buf_fd, EPOLLOUT);

    debug(std::string ("response") + response);
    debug("complete");
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
Connection Server::accept(int server_fd) {
    sockaddr_in client_addr{};
    socklen_t addr_size = sizeof(client_addr);
    Descriptor fd(::accept4(server_fd, reinterpret_cast<sockaddr*>(&client_addr), &addr_size, SOCK_NONBLOCK));
    if (fd < 0) {
        if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
            return Connection();
        }
        throw BaseException("accept4 failed");
    }
    Connection connection(std::move(fd));
    return connection;
}
std::string Server::read_full() { return std::string(); }

}  // namespace http
}  // namespace tp
