#include "Epoll.h"

#include "BaseException.h"

#include <sys/epoll.h>

namespace tp {
namespace http {
tp::http::Epoll::Epoll(size_t epoll_size) : _epoll_size(epoll_size) {
    Descriptor epoll_fd(epoll_create(1));
    if (epoll_fd < 0) {
        throw BaseException("epoll not created");
    }
    _epoll_fd = std::move(epoll_fd);
    epoll_events.resize(_epoll_size);
}

void Epoll::ctl(uint32_t cor, int fd, uint32_t events, int action) {
    epoll_event event{};
    event.data.u32 = cor;
    event.events = events;

    if (epoll_ctl(_epoll_fd.get(), action, fd, &event) < 0) {
        throw BaseException("epoll_ctl failed");
    }
}
int Epoll::wait() { return epoll_wait(_epoll_fd.get(), epoll_events.data(), _epoll_size, 50); }
}  // namespace http
}  // namespace tp