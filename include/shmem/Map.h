#ifndef INCLUDE_SHMEM_MAP_H_
#define INCLUDE_SHMEM_MAP_H_

#include "Shalloc.h"
#include <sys/mman.h>
#include <map>

namespace shmem {

using string = std::basic_string<char, std::char_traits<char>, Shalloc<char>>;

template <typename Key, typename T, typename Compare = std::less<Key>>
class Map {
 public:
    using elem = std::pair<const Key, T>;
    using TempMap = std::map<Key, T, Compare, Shalloc<elem>>;

    explicit Map(size_t size);
    ~Map();

    void insert(std::pair<Key, T> pair);
    void erase(Key key);
    T& operator[](Key key);
    TempMap& operator*();

    sem_t * get_sem();


 private:
    TempMap *_map;
    size_t mmap_size;
    sem_t *_semaphore;
};

template<typename Key, typename T, typename Compare>
Map<Key, T, Compare>::Map(size_t size) {
    mmap_size = size * (sizeof(std::pair<Key, T>) + sizeof(std::_Rb_tree_node_base))
              + sizeof(TempMap)
              + sizeof(AllocState)
              + sizeof(sem_t);

    void *mmap = ::mmap(nullptr,
                        mmap_size,
                        PROT_READ | PROT_WRITE,
                        MAP_SHARED | MAP_ANONYMOUS,
                        -1,
                        0);

    if (mmap == MAP_FAILED) {
        throw Exception("Error creaing mmap");
    }

    auto *state = static_cast<AllocState *>(mmap);
    state->start = static_cast<char *>(mmap) + sizeof(*state);
    state->end = static_cast<char *>(mmap) + mmap_size;

    auto sem = reinterpret_cast<sem_t *>(state->start);
    ::sem_init(sem, 1, 1);
    state->start = static_cast<char *>(state->start) + sizeof(sem_t);

    auto map = reinterpret_cast<TempMap *>(state->start);
    state->start = static_cast<char *>(state->start) + sizeof(TempMap);

    _map = new (map) TempMap{Shalloc<elem>(state)};
    _semaphore = sem;
}

template<typename Key, typename T, typename Compare>
void Map<Key, T, Compare>::insert(std::pair<Key, T> pair) {
    _map->insert(pair);
}

template<typename Key, typename T, typename Compare>
void Map<Key, T, Compare>::erase(Key key) {
    _map->erase(key);
}

template<typename Key, typename T, typename Compare>
T &Map<Key, T, Compare>::operator[](Key key) {
    return _map[key];
}

template<typename Key, typename T, typename Compare>
Map<Key, T, Compare>::~Map() {
    munmap(_map, mmap_size);
}

template<typename Key, typename T, typename Compare>
sem_t * Map<Key, T, Compare>::get_sem() {
    return _semaphore;
}

template<typename Key, typename T, typename Compare>
std::map<Key, T, Compare, Shalloc<std::pair<const Key, T>>> &Map<Key, T, Compare>::operator*() {
    return *_map;
}
}  // namespace shmem

#endif  // INCLUDE_SHMEM_MAP_H_
