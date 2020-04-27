#ifndef SHMEM_SHALLOC_H
#define SHMEM_SHALLOC_H

#include <semaphore.h>
#include <exception>
#include <string>
#include <sys/mman.h>
#include <unistd.h>

namespace shmem {

//TODO: create separate class for exception after merge hw-5 (because will be many change in pull request)
class Exception : public std::exception {
 public:
    explicit Exception(std::string error) : _error(std::move(error)) {
        _error += std::string(" : ");
    }

    [[nodiscard]] const char *what() const noexcept override {
        return _error.c_str();
    }

 private:
    std::string _error;
};

struct AllocState {
    char *start;
    char *end;
};


class SharedLinearMemory {
 public:
    static SharedLinearMemory &get_instance(size_t size = 1024) {
        static SharedLinearMemory alloc(size);
        return alloc;
    }

    AllocState *get_state() {
        return _state;
    }

    sem_t *get_semaphore() {
        return _semaphore;
    }


    SharedLinearMemory(const SharedLinearMemory &) = delete;

    SharedLinearMemory &operator=(const SharedLinearMemory &) = delete;

 private:
    SharedLinearMemory(size_t size) {
        _parent_pid = getpid();
        _mmap_size = sizeof(AllocState)
                     + sizeof(sem_t)
                     + sizeof(pid_t)
                     + size;

        _mmap = ::mmap(nullptr,
                       _mmap_size,
                       PROT_READ | PROT_WRITE,
                       MAP_SHARED | MAP_ANONYMOUS,
                       -1,
                       0);

        if (_mmap == MAP_FAILED) {
            throw Exception("Error creaing mmap");
        }

        _state = static_cast<AllocState *>(_mmap);
        _state->start = static_cast<char *>(_mmap) + sizeof(*_state);
        _state->end = static_cast<char *>(_mmap) + _mmap_size;

        _semaphore = reinterpret_cast<sem_t *>(_state->start);
        ::sem_init(_semaphore, 1, 1);
        _state->start = static_cast<char *>(_state->start) + sizeof(sem_t);
    }

    ~SharedLinearMemory() {
        if (getpid() == _parent_pid) {
            munmap(_mmap, _mmap_size);
        }
    }

 private:
    size_t _mmap_size;
    AllocState *_state;
    void *_mmap;
    pid_t _parent_pid;
    sem_t *_semaphore;
};

template<typename T>
class Shalloc {
 public:
    using value_type = T;

    Shalloc() : _state(SharedLinearMemory::get_instance().get_state()) {}

    template<typename U>
    explicit Shalloc(const Shalloc<U> &other) noexcept : _state(SharedLinearMemory::get_instance().get_state()) {}

    T *allocate(size_t n) {
        char *res = _state->start;
        if (res + sizeof(T) * n > _state->end) {
            throw std::bad_alloc{};
        }
        _state->start += sizeof(T) * n;
        return reinterpret_cast<T *>(res);
    }

    void deallocate(T *ptr, size_t n) noexcept {
        if (_state->start - sizeof(T) * n == reinterpret_cast<char *>(ptr)) {
            _state->start -= sizeof(T) * n;
        }
    }

    template<typename U>
    friend bool operator==(const Shalloc<T> &lalloc, const Shalloc<U> &ralloc);

 private:
    AllocState *_state;
};

template<typename T, typename U>
bool operator==(const Shalloc<T> &lalloc, const Shalloc<U> &ralloc) { return lalloc._state == ralloc._state; }


class SemLock {
 public:
    explicit SemLock(sem_t *semaphore) : _sem(semaphore) { ::sem_wait(_sem); }

    ~SemLock() { sem_post(_sem); }

 private:
    sem_t *_sem;
};
}  // namespace shmem

#endif  // SHMEM_SHALLOC_H
