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
    _fd = sock.extract();

    set_max_connect(10);
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

ConnectionPtr Server::accept() {
    sockaddr_in client_addr{};
    socklen_t addr_size = sizeof(client_addr);
    process::Descriptor fd{::accept(_fd.get(), reinterpret_cast<sockaddr *>(&client_addr), &addr_size)};
    if (fd < 0) {
        throw TcpException("accept failed");
    }

    sockaddr_in server_addr{};
    int server_addr_size = sizeof(server_addr);
    if (getsockname(fd.get(),
                    reinterpret_cast<sockaddr *>(&server_addr),
                    reinterpret_cast<socklen_t *>(&server_addr_size)) < 0) {
        throw TcpException("client data not received");
    }

    std::string client_ip = inet_ntoa(client_addr.sin_addr);
    uint16_t client_port = ntohs(client_addr.sin_port);
    std::string server_ip = inet_ntoa(server_addr.sin_addr);
    uint16_t server_port = ntohs(server_addr.sin_port);

    return std::unique_ptr<Connection>(new Connection(fd.extract(), client_ip, client_port, server_ip, server_port));
}
}  // namespace tcp
