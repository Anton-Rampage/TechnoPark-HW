#include "Connection.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>


namespace tcp {
TcpException::TcpException(std::string error) : _error(std::move(error)) {
    _error += std::string(" : ") + strerror(errno);
}

const char *TcpException::what() const noexcept {
    return _error.c_str();
}


Connection::Connection(const std::string &ip, int port) {
    connect(ip, port);
}

void Connection::connect(const std::string& ip, int port) {
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        throw TcpException("socket not created");
    }
    close();
    _fd = fd;
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_aton(ip.c_str(), &addr.sin_addr);

    if (::connect(_fd.get(), reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
        throw TcpException("not connected");
    }

    sockaddr_in client_addr{};
    int client_addr_size = sizeof(client_addr);
    if (getsockname(_fd.get(),
                    reinterpret_cast<sockaddr *>(&client_addr),
                    reinterpret_cast<socklen_t *>(&client_addr_size)) < 0) {
        throw TcpException("client info not received");
    }
    set_timeout(2);

    _dst_ip = ip;
    _dst_port = port;
    _src_ip = inet_ntoa(client_addr.sin_addr);
    _src_port = ntohs(client_addr.sin_port);
    _readable = true;
}

void Connection::close() {
    _fd.close();
    _readable = false;
}

Connection::~Connection() noexcept {
    close();
}

std::string Connection::get_src_ip() const {
    return _src_ip;
}

uint16_t Connection::get_src_port() const {
    return _src_port;
}

std::string Connection::get_dst_ip() const {
    return _dst_ip;
}

uint16_t Connection::get_dst_port() const {
    return _dst_port;
}

size_t Connection::write(const void *data, size_t len) {
    auto size = ::write(_fd.get(), data, len);
    if (size == -1) {
        throw TcpException("write failed");
    }
    return size;
}

size_t Connection::read(void *data, size_t len) {
    auto size = ::read(_fd.get(), data, len);
    if (size == -1) {
        throw TcpException("read failed");
    }
    return size;
}

void Connection::write_exact(const void *data, size_t len) {
    size_t write_length = 0;
    while (write_length < len) {
        write_length += write(static_cast<const char *>(data) + write_length, len - write_length);
    }
}

void Connection::read_exact(void *data, size_t len) {
    size_t write_length = 0;
    while (write_length < len) {
        write_length += read(static_cast<char *>(data) + write_length, len - write_length);
    }
}

bool Connection::is_readable() {
    return _readable;
}

Connection::Connection(int fd, std::string dst_ip, int dst_port,
                       std::string src_ip, int src_port)
    : _fd(fd)
    , _dst_ip(std::move(dst_ip)), _dst_port(dst_port)
    , _src_ip(std::move(src_ip)), _src_port(src_port)
    , _readable(true) {}

void Connection::set_timeout(int num) {
    timeval timeout{.tv_sec = num,
                    .tv_usec = 0};

    if (setsockopt(_fd.get(),
                   SOL_SOCKET,
                   SO_SNDTIMEO,
                   &timeout,
                   sizeof(timeout)) < 0) {
        throw TcpException("set timeout for write failed");
    }

    if (setsockopt(_fd.get(),
                   SOL_SOCKET,
                   SO_RCVTIMEO,
                   &timeout,
                   sizeof(timeout)) < 0) {
        throw TcpException("set timeout for read failed");
    }
}

}  // namespace tcp
