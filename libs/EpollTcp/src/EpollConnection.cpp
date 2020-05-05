#include "EpollConnection.h"

#include "BaseException.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

namespace tp {
namespace epolltcp {

EpollConnection::EpollConnection(const std::string &ip, int port) { connect(ip, port); }

void EpollConnection::connect(const std::string &ip, int port) {
    Descriptor sock(socket(PF_INET, SOCK_STREAM, 0));
    if (sock < 0) {
        throw BaseException("socket not created");
    }
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_aton(ip.c_str(), &addr.sin_addr);

    if (::connect(sock.get(), reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
        throw BaseException("not Epollconnected");
    }

    sockaddr_in client_addr{};
    socklen_t client_addr_size = sizeof(client_addr);
    if (getsockname(sock.get(), reinterpret_cast<sockaddr *>(&client_addr), &client_addr_size) < 0) {
        throw BaseException("client info not received");
    }
    _fd = sock.extract();
    _dst_ip = ip;
    _dst_port = port;
    _src_ip = inet_ntoa(client_addr.sin_addr);
    _src_port = ntohs(client_addr.sin_port);
    _readable = true;
    _writable = true;
}

void EpollConnection::close() {
    _fd.close();
    _readable = false;
}

EpollConnection::~EpollConnection() noexcept { close(); }

std::string EpollConnection::get_src_ip() const { return _src_ip; }

uint16_t EpollConnection::get_src_port() const { return _src_port; }

std::string EpollConnection::get_dst_ip() const { return _dst_ip; }

uint16_t EpollConnection::get_dst_port() const { return _dst_port; }

size_t EpollConnection::write(const void *data, size_t len) {
    size_t size = ::write(_fd.get(), data, len);
    if (size == -1) {
        throw BaseException("write failed");
    }
    del_cache_write(size);
    return size;
}

size_t EpollConnection::read(void *data, size_t len) {
    size_t size = ::read(_fd.get(), data, len);
    if (size == -1) {
        throw BaseException("read failed");
    }
    if (size == 0) {
        _readable = false;
        return 0;
    }
    add_cache_read(data, size);
    return size;
}

bool EpollConnection::is_readable() { return _readable; }

EpollConnection::EpollConnection(Descriptor &&fd) : _fd(std::move(fd)), _readable(true), _writable(true) {
    sockaddr_in self_addr{};
    socklen_t self_addr_size = sizeof(self_addr);
    if (getsockname(_fd.get(), reinterpret_cast<sockaddr *>(&self_addr), &self_addr_size) < 0) {
        throw BaseException("getsockname failed");
    }

    sockaddr_in con_addr{};
    socklen_t con_addr_size = sizeof(self_addr);
    if (getpeername(_fd.get(), reinterpret_cast<sockaddr *>(&con_addr), &con_addr_size) < 0) {
        throw BaseException("getpeername failed");
    }

    _dst_ip = inet_ntoa(con_addr.sin_addr);
    _dst_port = ntohs(con_addr.sin_port);
    _src_ip = inet_ntoa(self_addr.sin_addr);
    _src_port = ntohs(self_addr.sin_port);
}

void EpollConnection::set_timeout(int num) {
    timeval timeout{.tv_sec = num, .tv_usec = 0};

    if (setsockopt(_fd.get(), SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        throw BaseException("set timeout for write failed");
    }

    if (setsockopt(_fd.get(), SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        throw BaseException("set timeout for read failed");
    }
}

EpollConnection &EpollConnection::operator=(EpollConnection &&new_con) noexcept {
    if (this == &new_con) {
        return *this;
    }
    close();
    _fd = std::move(new_con._fd);
    _dst_ip = new_con.get_dst_ip();
    _dst_port = new_con.get_dst_port();
    _src_ip = new_con.get_src_ip();
    _src_port = new_con.get_src_port();
    _readable = new_con._readable;
    _writable = new_con._writable;
    new_con.close();
    return *this;
}

EpollConnection::EpollConnection(EpollConnection &&new_con) noexcept {
    if (this == &new_con) {
        return;
    }
    close();
    _fd = std::move(new_con._fd);
    _dst_ip = new_con.get_dst_ip();
    _dst_port = new_con.get_dst_port();
    _src_ip = new_con.get_src_ip();
    _src_port = new_con.get_src_port();
    _readable = new_con._readable;
    _writable = new_con._writable;
    new_con.close();
}
char *EpollConnection::get_cache_read() { return _cache_read.data(); }

char *EpollConnection::get_cache_write() { return _cache_write.data(); }

void EpollConnection::set_cache_size(size_t read, size_t write, void *write_data) {
    _cache_read_size = read;
    _cache_write_size = write;
    _cache_write = std::vector<char>(static_cast<char *>(write_data), static_cast<char *>(write_data) + write);
}

void EpollConnection::add_cache_read(void *add, size_t size) {
    _cache_read.insert(_cache_read.end(), static_cast<char *>(add), static_cast<char *>(add) + size);
    _cache_read_size -= size;
    if (_cache_read_size == 0) {
        _readable = false;
    }
}

void EpollConnection::del_cache_write(size_t size) {
    std::vector new_buf(_cache_write.begin() + size, _cache_write.end());
    _cache_write.swap(new_buf);
    _cache_write_size -= size;
    if (_cache_write_size == 0) {
        _writable = false;
    }
}

size_t EpollConnection::get_cache_size_read() const { return _cache_read_size; }

size_t EpollConnection::get_cache_size_write() const { return _cache_write_size; }

bool EpollConnection::is_writable() const { return _writable; }

}  // namespace epolltcp
}  // namespace tp
