#include "Connection.h"

#include "BaseException.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>
#include <vector>

namespace tp {
namespace http {
// Connection::Connection(const std::string &ip, int port) { connect(ip, port); }

// void Connection::connect(const std::string &ip, int port) {
//    Descriptor sock(socket(PF_INET, SOCK_STREAM, 0));
//    if (sock < 0) {
//        throw BaseException("socket not created");
//    }
//    sockaddr_in addr{};
//    addr.sin_family = AF_INET;
//    addr.sin_port = htons(port);
//    inet_aton(ip.c_str(), &addr.sin_addr);
//
//    if (::connect(sock.get(), reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
//        throw BaseException("not connected");
//    }
//
//    sockaddr_in client_addr{};
//    socklen_t client_addr_size = sizeof(client_addr);
//    if (getsockname(sock.get(), reinterpret_cast<sockaddr *>(&client_addr), &client_addr_size) < 0) {
//        throw BaseException("client info not received");
//    }
//    _fd = sock.extract();
//    _readable = true;
//}

void Connection::close() {
    _fd.close();
    _readable = false;
}

size_t Connection::write(const void *data, size_t len) {
    size_t size = ::write(_fd.get(), data, len);
    if (size == -1) {
        throw BaseException("write failed");
    }
    return size;
}

size_t Connection::read(void *data, size_t len) {
    size_t size = ::read(_fd.get(), data, len);
    if (size == -1 && errno != EAGAIN) {
        throw BaseException("read failed");
    }
    if (size == 0) {
        _readable = false;
    }
    return size;
}

// void Connection::write_exact(const void *data, size_t len) {
//    ssize_t write_length = 0;
//    while (write_length < len) {
//        write_length += write(static_cast<const char *>(data) + write_length, len - write_length);
//    }
//}
//
// void Connection::read_exact(void *data, size_t len) {
//    ssize_t read_length = 0;
//    while (_readable && read_length < len) {
//        read_length += read(static_cast<char *>(data) + read_length, len - read_length);
//    }
//}

bool Connection::is_readable() const { return _readable; }

Connection::Connection(Descriptor &&fd) : _fd(std::move(fd)), _readable(true) {}

Connection &Connection::operator=(Connection &&new_con) noexcept {
    if (this == &new_con) {
        return *this;
    }
    close();
    _fd = std::move(new_con._fd);
    _readable = new_con._readable;
    new_con.close();
    return *this;
}

Connection::Connection(Connection &&new_con) noexcept {
    if (this == &new_con) {
        return;
    }
    close();
    _fd = std::move(new_con._fd);
    _readable = new_con._readable;
    new_con.close();
}
}  // namespace http
}  // namespace tp
