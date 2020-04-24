#ifndef INCLUDE_SHMEM_MAP_H_
#define INCLUDE_SHMEM_MAP_H_

#include "Shalloc.h"
#include <sys/mman.h>
#include <map>
#include <iostream>

namespace shmem {

using string = std::basic_string<char, std::char_traits<char>, Shalloc<char>>;

template <typename Key, typename T, typename Compare = std::less<Key>>
class Map {
 public:
    using elem = std::pair<const Key, T>;
    using TempMap = std::map<Key, T, Compare, Shalloc<elem>>;

    explicit Map(size_t size);
    ~Map() noexcept { munmap(_mmap, mmap_size); }

    void insert(elem&& pair) { _map->insert(std::forward<elem&&>(pair)); }
    void erase(const Key& key) { return _map->erase(key); }
    T& operator[](Key&& key) { return (*_map)[std::forward<Key&&>(key)]; }

    auto begin() { return _map->begin(); }
    auto end() { return _map->end(); }
    auto cbegin() { return _map->cbegin(); }
    auto cend() { return _map->cend(); }

    sem_t * get_sem() { return _semaphore; }


 private:
    TempMap *_map;
    size_t mmap_size;
    sem_t *_semaphore;
    void *_mmap;  // save it to free in destructor
};

template<typename Key, typename T, typename Compare>
Map<Key, T, Compare>::Map(size_t size) {
    mmap_size = size * sizeof(std::_Rb_tree_node_base)
              + sizeof(TempMap)
              + sizeof(AllocState)
              + sizeof(sem_t)
              + size;

    _mmap = ::mmap(nullptr,
                        mmap_size,
                        PROT_READ | PROT_WRITE,
                        MAP_SHARED | MAP_ANONYMOUS,
                        -1,
                        0);

    if (_mmap == MAP_FAILED) {
        throw Exception("Error creaing mmap");
    }

    auto *state = static_cast<AllocState *>(_mmap);
    state->start = static_cast<char *>(_mmap) + sizeof(*state);
    state->end = static_cast<char *>(_mmap) + mmap_size;

    auto sem = reinterpret_cast<sem_t *>(state->start);
    ::sem_init(sem, 1, 1);
    state->start = static_cast<char *>(state->start) + sizeof(sem_t);

    auto map = reinterpret_cast<TempMap *>(state->start);
    state->start = static_cast<char *>(state->start) + sizeof(TempMap);

    _map = new(map) TempMap{Shalloc<elem>(state)};

    _semaphore = sem;
}

}  // namespace shmem

#endif  // INCLUDE_SHMEM_MAP_H_
