#include "Process.h"

#include "BaseException.h"

#include <cstring>
#include <sys/wait.h>
#include <unistd.h>

namespace tp::process {
Process::Process(const std::string &path) : _status(-1) {
    Pipe in{};
    Pipe out{};

    _pid = fork();

    if (_pid < 0) {
        throw BaseException("process not created");
    } else if (_pid == 0) {
        if (dup2(out[WRITE], STDOUT_FILENO) == -1) {
            throw BaseException("dup2 out failed");
        }
        if (dup2(in[READ], STDIN_FILENO) == -1) {
            throw BaseException("dup2 in failed");
        }

        in.close();
        out.close();

        if (execlp(path.c_str(), path.c_str(), nullptr) == -1) {
            throw BaseException("execlp failed");
        }
    } else {
        _read_fd = out.move(READ);
        _write_fd = in.move(WRITE);
    }
}

Process::~Process() noexcept { waitpid(_pid, &_status, 0); }

size_t Process::write(const void *data, size_t len) {
    auto size = ::write(_write_fd.get(), data, len);
    if (size == -1) {
        throw BaseException("write failed");
    }
    return size;
}

void Process::write_exact(const void *data, size_t len) {
    size_t write_length = 0;
    while (write_length < len) {
        write_length += write(static_cast<const char *>(data) + write_length, len - write_length);
    }
}

size_t Process::read(void *data, size_t len) {
    auto size = ::read(_read_fd.get(), data, len);
    if (size == -1) {
        throw BaseException("read failed");
    }
    if (size == 0) {
        _readable = false;
    }
    return size;
}

void Process::read_exact(void *data, size_t len) {
    size_t read_length = 0;
    while (_readable && read_length < len) {
        read_length += read(static_cast<char *>(data) + read_length, len - read_length);
    }
}

bool Process::is_readable() const { return _readable; }

void Process::close_stdin() { _write_fd.close(); }

void Process::close() const { kill(_pid, SIGTERM); }

Pipe::Pipe() {
    int fd[2]{};
    if (pipe(fd) != 0) {
        throw BaseException("pipe not created");
    }

    _read = fd[READ];
    _write = fd[WRITE];
}

Pipe::~Pipe() noexcept { close(); }

int Pipe::move(IO io) {
    if (io == READ) {
        return _read.extract();
    }
    return _write.extract();
}

int Pipe::operator[](IO io) {
    if (io == READ) {
        return _read.get();
    }
    return _write.get();
}

void Pipe::close() {
    _read.close();
    _write.close();
}
}  // namespace tp::process
