#ifndef APPLICATION_API_CONTAINERS_HASHMAP_HPP_
#define APPLICATION_API_CONTAINERS_HASHMAP_HPP_
#include "../Memory/VAllocator.hpp"
#include <unordered_map>

namespace api {
template <class Key, class Value, class Hash = std::hash<Key>,
          class Comparator = std::equal_to<Key>,
          class Allocator = std::allocator<
              std::pair<const Key, Value>> /*api::VAllocator<std::pair<const
                                              Key, Value>*/
          >
using HashMap = std::unordered_map<Key, Value, Hash, Comparator, Allocator>;
}
#endif // !APPLICATION_API_CONTAINERS_HASHMAP_HPP_
