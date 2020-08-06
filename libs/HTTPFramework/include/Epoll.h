#ifndef INCLUDE_HTTPFRAMEWORK_EPOLL_H_
#define INCLUDE_HTTPFRAMEWORK_EPOLL_H_

#include "Descriptor.h"

#include <sys/epoll.h>
#include <vector>

namespace tp {
namespace http {

class Epoll {
 public:
    Epoll(size_t epoll_size);
    ~Epoll() = default;
    inline void add(uint32_t cor, int fd, uint32_t events) { ctl(cor, fd, events, EPOLL_CTL_ADD); }
    inline void del(uint32_t cor, int fd, uint32_t events) { ctl(cor, fd, events, EPOLL_CTL_DEL); }
    inline void mod(uint32_t cor, int fd, uint32_t events) { ctl(cor, fd, events, EPOLL_CTL_MOD); }
    int wait();
    std::vector<epoll_event> epoll_events;

 private:
    void ctl(uint32_t cor, int fd, uint32_t events, int action);

 private:
    Descriptor _epoll_fd;
    size_t _epoll_size;
};

}  // namespace http
}  // namespace tp

#endif  // INCLUDE_HTTPFRAMEWORK_EPOLL_H_
