#ifndef APPLICATION_API_DATASTRUCTURES_CONTAINERS_HASHSET_HPP_
#define APPLICATION_API_DATASTRUCTURES_CONTAINERS_HASHSET_HPP_

// STL
#include <unordered_set>

namespace api {
template <class Key, class Hash = std::hash<Key>,
          class Comparator = std::equal_to<Key>,
          class Allocator = std::allocator<Key>>
using HashSet = std::unordered_set<Key, Hash, Comparator, Allocator>;
}
#endif // !APPLICATION_API_DATASTRUCTURES_CONTAINERS_HASHSET_HPP_
