#ifndef INCLUDE_SHMEM_MAP_H_
#define INCLUDE_SHMEM_MAP_H_

#include "Shalloc.h"
#include <sys/mman.h>
#include <map>
#include <iostream>
#include <scoped_allocator>
#include <memory>


namespace shmem {

using string = std::basic_string<char, std::char_traits<char>, Shalloc<char>>;

template<typename Key, typename T, typename Compare = std::less<Key>>
class Map {
 public:
    using elem = std::pair<const Key, T>;
    using TempMap = std::map<Key, T, Compare, Shalloc<T>>;

    explicit Map() {
        _parent_pid = getpid();
        auto state = SharedLinearMemory::get_instance().get_state();
        auto map = reinterpret_cast<TempMap *>(state->start);
        state->start = static_cast<char *>(state->start) + sizeof(TempMap);
        auto alloc = Shalloc<elem>();
        _map = new(map) TempMap{};
    }

    ~Map() noexcept {
        if (getpid() == _parent_pid) {
            std::destroy_at(_map);
        }
    }

    void insert(elem &&pair) {
        SemLock lock(SharedLinearMemory::get_instance().get_semaphore());
        _map->insert(std::forward<elem>(pair));
    }

    void erase(const Key &key) {
        SemLock lock(SharedLinearMemory::get_instance().get_semaphore());
        return _map->erase(key);
    }

    T &operator[](Key &&key) {
        SemLock lock(SharedLinearMemory::get_instance().get_semaphore());
        return (*_map)[std::forward<Key>(key)];
    }

    auto begin() { return _map->begin(); }

    auto end() { return _map->end(); }

    auto cbegin() { return _map->cbegin(); }

    auto cend() { return _map->cend(); }

 private:
    TempMap *_map;
    size_t mmap_size;
    void *_mmap;  // save it to free in destructor
    pid_t _parent_pid;
};


}  // namespace shmem

#endif  // INCLUDE_SHMEM_MAP_H_
