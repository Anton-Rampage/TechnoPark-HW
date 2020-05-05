#ifndef INCLUDE_PROCESS_PROCESS_H_
#define INCLUDE_PROCESS_PROCESS_H_

#include "Descriptor.h"

#include <exception>
#include <string>

namespace tp::process {
enum IO : int { READ = 0, WRITE = 1 };

class Pipe {
 public:
    Pipe();
    ~Pipe() noexcept;
    int move(IO io);
    void close();
    int operator[](IO io);

 private:
    Descriptor _read;
    Descriptor _write;
};

class Process {
 public:
    explicit Process(const std::string& path);
    ~Process() noexcept;

    size_t write(const void* data, size_t len);
    void write_exact(const void* data, size_t len);

    size_t read(void* data, size_t len);
    void read_exact(void* data, size_t len);

    [[nodiscard]] bool is_readable() const;

    void close_stdin();
    void close() const;

 private:
    pid_t _pid;
    Descriptor _read_fd;
    Descriptor _write_fd;
    int _status;
    bool _readable = true;
};
}  // namespace tp::process

#endif  // INCLUDE_PROCESS_PROCESS_H_
