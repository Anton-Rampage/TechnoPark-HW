#ifndef INCLUDE_PROCESS_PROCESS_H_
#define INCLUDE_PROCESS_PROCESS_H_

#include <string>
#include <exception>
#include "Descriptor.h"

namespace process {
enum IO : int {
    READ = 0,
    WRITE = 1
};

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


class ProcessException : public std::exception {
 public:
    explicit ProcessException(std::string error);
    const char* what() const noexcept override;
 private:
    std::string _error;
};


class Process {
 public:
    explicit Process(const std::string& path);
    ~Process() noexcept;
    size_t write(const void* data, size_t len);
    void write_exact(const void* data, size_t len);
    size_t read(void* data, size_t len);
    void read_exact(void* data, size_t len);
    bool is_readable() const;
    void close_stdin();
    void close();
 private:
    pid_t _pid;
    Descriptor _read_fd;
    Descriptor _write_fd;
    int _status;
};
}  // namespace process

#endif  // INCLUDE_PROCESS_PROCESS_H_
