#ifndef SHMEM_SHALLOC_H
#define SHMEM_SHALLOC_H

#include <semaphore.h>
#include <exception>
#include <string>

namespace shmem {

struct AllocState {
    char *start;
    char *end;
};

template <typename T>
class Shalloc {
 public:
    typedef T value_type;
    Shalloc() noexcept {}
    Shalloc(AllocState *state);
    template <typename U>
    explicit Shalloc<T>(const Shalloc<U>& other) noexcept {
        this->_state = other.get();
    }
    T * allocate(size_t n);
    void deallocate(T *ptr, size_t n) noexcept;
    AllocState * get() const;

 private:
    AllocState *_state = nullptr;
};

class SemLock {
 public:
    explicit SemLock(sem_t* semaphore);
    ~SemLock();

 private:
    sem_t* _sem;
};

//TODO: create separate class for exception after merge hw-5 (because will be many change in pull request)
class Exception : public std::exception {
 public:
    explicit Exception(std::string error) : _error(std::move(error)) {
        _error += std::string(" : ");
    }
    [[nodiscard]] const char* what() const noexcept override {
        return _error.c_str();
    }

 private:
    std::string _error;
};


template<typename T>
Shalloc<T>::Shalloc(AllocState *state) {
    _state = state;
}

template<typename T>
T *Shalloc<T>::allocate(size_t n) {
    char *res = _state->start;
    if (res + sizeof(T) * n > _state->end) {
        throw std::bad_alloc{};
    }
    _state->start += sizeof(T) * n;
    return reinterpret_cast<T *>(res);
}

template<typename T>
void Shalloc<T>::deallocate(T *ptr, size_t n) noexcept {
    if (_state->start - sizeof(T) * n == reinterpret_cast<char *>(ptr)) {
        _state->start -= sizeof(T) * n;
    }
}

template<typename T>
AllocState *Shalloc<T>::get() const {
    return _state;
}

SemLock::SemLock(sem_t *semaphore) : _sem(semaphore) {
    ::sem_wait(_sem);
}

SemLock::~SemLock() {
    sem_post(_sem);
}



}  // namespace shmem


#endif //SHMEM_SHALLOC_H
