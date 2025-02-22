#include "Descriptor.h"

#include <unistd.h>

namespace tp {
Descriptor::Descriptor(int fd) : _fd(fd) {}

Descriptor::~Descriptor() noexcept { close(); }

void Descriptor::close() {
    if (_fd != -1) {
        ::close(_fd);
        _fd = -1;
    }
}

int Descriptor::get() const { return _fd; }

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

Descriptor::Descriptor(Descriptor &&new_fd) noexcept {
    close();
    _fd = new_fd.extract();
}

bool Descriptor::operator<(int num) const { return _fd < num; }

Descriptor &Descriptor::operator=(Descriptor &&new_fd) noexcept {
    close();
    _fd = new_fd.extract();
    return *this;
}

}  // namespace tp
