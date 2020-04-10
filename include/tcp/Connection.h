#ifndef INCLUDE_TCP_CONNECTION_H_
#define INCLUDE_TCP_CONNECTION_H_

#include <string>
#include "Descriptor.h"

namespace tcp {
class TcpException : public std::exception {
 public:
    explicit TcpException(std::string error);
    const char* what() const noexcept override;
 private:
    std::string _error;
};


class Connection {
 public:
    Connection() = default;
    Connection(const std::string& ip, int port);
    void connect(const std::string& ip, int port);

    ~Connection() noexcept;
    void close();

    Connection& operator=(Connection&& new_con) noexcept;
    Connection(Connection&& new_con) noexcept;
    Connection(Connection&) = delete;
    Connection& operator=(Connection& new_con) = delete;

    size_t write(const void *data, size_t len);
    size_t read(void *data, size_t len);
    void write_exact(const void *data, size_t len);
    void read_exact(void *data, size_t len);

    void set_timeout(int num);

    bool is_readable();

    std::string get_src_ip() const;
    uint16_t get_src_port() const;
    std::string get_dst_ip() const;
    uint16_t get_dst_port() const;

    void * get_cache();

 private:
    friend class Server;
    friend class EpollServer;
    Connection(process::Descriptor&& fd);


    process::Descriptor _fd;
    std::string _src_ip;
    uint16_t _src_port = 0;
    std::string _dst_ip;
    uint16_t _dst_port = 0;
    bool _readable = false;

    void *_cache = nullptr;
};
}  // namespace tcp

#endif  // INCLUDE_TCP_CONNECTION_H_
