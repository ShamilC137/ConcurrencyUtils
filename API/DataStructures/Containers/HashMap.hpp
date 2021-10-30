#ifndef APPLICATION_API_DATASTRUCTURES_CONTAINERS_HASHMAP_HPP_
#define APPLICATION_API_DATASTRUCTURES_CONTAINERS_HASHMAP_HPP_

// STL
#include <unordered_map>

namespace api {
template <class Key, class T, class Hash = std::hash<Key>,
          class KeyEqual = std::equal_to<Key>,
          class Allocator = std::allocator<std::pair<const Key, T>>>
using HashMap = std::unordered_map<Key, T, Hash, KeyEqual, Allocator>;
}
#endif // !APPLICATION_API_DATASTRUCTURES_CONTAINERS_HASHMAP_HPP_
