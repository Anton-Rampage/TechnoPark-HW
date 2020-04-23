#include "Server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


namespace tcp {
Server::~Server() {
    close();
}

Server::Server(const std::string &ip, int port) {
    open(ip, port);
}

void Server::open(const std::string &ip, int port) {
    process::Descriptor sock(socket(PF_INET, SOCK_STREAM, 0));
    if (sock < 0) {
        throw TcpException("socket not created");
    }
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_aton(ip.c_str(), &addr.sin_addr);

    if (bind(sock.get(), reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
        throw TcpException("bind failed");
    }

    if (listen(sock.get(), 10) < 0) {
        throw TcpException("listen failed");
    }

    _fd = sock.extract();
    _opened = true;
}

void Server::close() {
    _fd.close();
    _opened = false;
}

bool Server::is_opened() {
    return _opened;
}

void Server::set_max_connect(int num) {
    if (listen(_fd.get(), num) < 0) {
        throw TcpException("listen failed");
    }
}

Connection Server::accept() {
    sockaddr_in client_addr{};
    socklen_t addr_size = sizeof(client_addr);
    process::Descriptor fd{::accept(_fd.get(), reinterpret_cast<sockaddr *>(&client_addr), &addr_size)};
    if (fd < 0) {
        throw TcpException("accept failed");
    }
    return Connection(std::move(fd));
}
}  // namespace tcp
