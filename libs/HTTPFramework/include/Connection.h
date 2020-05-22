#ifndef TECHNOPARK_CONNECTION_H
#define TECHNOPARK_CONNECTION_H

#include <string>
#include "Descriptor.h"

namespace tp {
namespace http {
class Connection {
 public:
    Connection() = default;
    void close();

    Connection& operator=(Connection&& new_con) noexcept;
    Connection(Connection&& new_con) noexcept;
    Connection(Connection&) = delete;
    Connection& operator=(Connection& new_con) = delete;

    size_t write(const void* data, size_t len);
    size_t read(void* data, size_t len);
//    void write_exact(const void* data, size_t len);
//    void read_exact(void* data, size_t len);

    [[nodiscard]] bool is_readable() const;
    explicit Connection(Descriptor&& fd);

 private:
    friend class Server;


    Descriptor _fd;
    bool _readable = false;

};
}
}

#endif  // TECHNOPARK_CONNECTION_H
