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
    Connection(const std::string& ip, int port);
    void connect(const std::string& ip, int port);

    ~Connection() noexcept;
    void close();

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

 private:
    friend class Server;
    Connection(int fd, std::string dst_ip, int dst_port,
                       std::string src_ip, int src_port);

    process::Descriptor _fd;
    std::string _src_ip;
    uint16_t _src_port;
    std::string _dst_ip;
    uint16_t _dst_port;
    bool _readable;
};
}  // namespace tcp

#endif  // INCLUDE_TCP_CONNECTION_H_
