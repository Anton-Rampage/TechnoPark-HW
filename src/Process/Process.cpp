#include <Process.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <cstring>


namespace process {
Process::Process(const std::string &path) : _status(-1) {
    Pipe in{};
    Pipe out{};

    _pid = fork();

    if (_pid < 0) {
        throw ProcessException("process not created");
    } else if (_pid == 0) {
        if (dup2(out[WRITE], STDOUT_FILENO) == -1) {
            throw ProcessException("dup2 out failed");
        }
        if (dup2(in[READ], STDIN_FILENO) == -1) {
            throw ProcessException("dup2 in failed");
        }

        in.close();
        out.close();

        if (execlp(path.c_str(), path.c_str(), nullptr) == -1) {
            throw ProcessException("execlp failed");
        }
    } else {
        _read_fd = out.move(READ);
        _write_fd = in.move(WRITE);
    }
}

Process::~Process() {
    waitpid(_pid, &_status, 0);
}

size_t Process::write(const void *data, size_t len) {
    auto size = ::write(_write_fd.get(), data, len);
    if (size == -1) {
        throw ProcessException("write failed");
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
        throw ProcessException("read failed");
    }
    if (size == 0) {
        _read_fd.close();
    }
    return size;
}

void Process::read_exact(void *data, size_t len) {
    size_t read_length = 0;
    size_t size = 0;
    while (read_length < len) {
        size = read(static_cast<char *>(data) + read_length, len - read_length);
        if (size == 0) {
            return;
        }
        read_length += size;
    }
}

bool Process::is_readable() const {
    return _read_fd.get() != -1;
}

void Process::close_stdin() {
    _write_fd.close();
}

void Process::close() {
    kill(_pid, SIGTERM);
}

ProcessException::ProcessException(std::string error) : _error(std::move(error)) {
    _error += std::string(" : ") + strerror(errno);
}

const char *ProcessException::what() const noexcept {
    return _error.c_str();
}

Pipe::Pipe() {
    int fd[2]{};
    if (pipe(fd) != 0) {
        throw ProcessException("pipe not created");
    }

    _read = fd[READ];
    _write = fd[WRITE];
}

Pipe::~Pipe() {
    close();
}

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
}  // namespace process
