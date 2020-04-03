#include "Descriptor.h"
#include <unistd.h>

namespace process {
Descriptor::Descriptor(int fd) : _fd(fd) {}

Descriptor::~Descriptor() noexcept {
    close();
}

void Descriptor::close() {
    if (_fd != -1) {
        ::close(_fd);
        _fd = -1;
    }
}

int Descriptor::get() const {
    return _fd;
}

Descriptor &Descriptor::operator=(int fd) {
    close();
    _fd = fd;
    return *this;
}

Descriptor::Descriptor() : _fd(-1) {}

int Descriptor::extract() {
    int ret = _fd;
    _fd = -1;
    return ret;
}

Descriptor::Descriptor(Descriptor && new_fd)  :_fd(new_fd.extract()) {}

bool Descriptor::operator<(int num) {
    return _fd < num;
}

}  // namespace process
