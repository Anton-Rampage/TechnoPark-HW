#ifndef INCLUDE_TCP_CONNECTION_H_
#define INCLUDE_TCP_CONNECTION_H_

#include <string>
#include <vector>
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
    bool is_writable();

    std::string get_src_ip() const;
    uint16_t get_src_port() const;
    std::string get_dst_ip() const;
    uint16_t get_dst_port() const;

    void set_cache_size(size_t read, size_t write, void * write_data);
    size_t get_cache_size_read();
    size_t get_cache_size_write();
    char * get_cache_read();
    void add_cache_read(void *add, size_t size);
    char * get_cache_write();
    void del_cache_write(size_t size);

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
    bool _writable = false;

    std::vector<char> _cache_read;
    size_t _cache_read_size = 0;
    std::vector<char> _cache_write;
    size_t _cache_write_size = 0;
};
}  // namespace tcp

#endif  // INCLUDE_TCP_CONNECTION_H_
