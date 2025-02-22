#ifndef INCLUDE_SHMEM_MAP_H_
#define INCLUDE_SHMEM_MAP_H_

#include "Shalloc.h"

#include <iostream>
#include <map>
#include <memory>
#include <scoped_allocator>
#include <sys/mman.h>

namespace tp {
namespace shmem {

using string = std::basic_string<char, std::char_traits<char>, Shalloc<char>>;

template <typename Key, typename T, typename Compare = std::less<Key>>
class Map {
 public:
    using elem = std::pair<const Key, T>;
    using TempMap = std::map<Key, T, Compare, Shalloc<T>>;

    explicit Map() {
        _parent_pid = getpid();
        auto state = SharedLinearMemory::get_instance().get_state();
        _map = reinterpret_cast<TempMap *>(state->start);
        state->start = static_cast<char *>(state->start) + sizeof(TempMap);
        auto alloc = Shalloc<elem>();
        std::allocator_traits<Shalloc<elem>>::construct(alloc, _map);
    }

    ~Map() noexcept {
        if (getpid() == _parent_pid) {
            std::destroy_at(_map);
        }
    }

    void insert(const elem &pair) {
        SemLock lock(SharedLinearMemory::get_instance().get_semaphore());
        _map->insert(pair);
    }

    void insert(const elem &&pair) {
        SemLock lock(SharedLinearMemory::get_instance().get_semaphore());
        _map->insert(std::move(pair));
    }

    void erase(const Key &key) {
        SemLock lock(SharedLinearMemory::get_instance().get_semaphore());
        return _map->erase(key);
    }

    T &operator[](Key &&key) {
        SemLock lock(SharedLinearMemory::get_instance().get_semaphore());
        return (*_map)[std::move(key)];
    }

    T &operator[](Key &key) {
        SemLock lock(SharedLinearMemory::get_instance().get_semaphore());
        return (*_map)[key];
    }

    auto begin() { return _map->begin(); }

    auto end() { return _map->end(); }

    auto cbegin() { return _map->cbegin(); }

    auto cend() { return _map->cend(); }

 private:
    TempMap *_map;
    pid_t _parent_pid;
};
}  // namespace shmem
}  // namespace tp

#endif  // INCLUDE_SHMEM_MAP_H_
