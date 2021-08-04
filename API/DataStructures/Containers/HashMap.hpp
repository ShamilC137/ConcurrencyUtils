#ifndef APPLICATION_API_DATASTRUCTURES_CONTAINERS_HASHMAP_HPP_
#define APPLICATION_API_DATASTRUCTURES_CONTAINERS_HASHMAP_HPP_
// current project
#include "../../../Config.hpp"
#include "../../Memory/AlignedAllocator.hpp"

// STL
#include <unordered_map>

namespace api {
#if STL_ALLOCATOR_USAGE
template <class Key, class T, class Hash = std::hash<Key>,
          class KeyEqual = std::equal_to<Key>,
          class Allocator = std::allocator<std::pair<const Key, T>>>
using HashMap = std::unordered_map<Key, T, Hash, KeyEqual, Allocator>;
#elif ALIGNED_ALLOCATOR_USAGE
template <class Key, class T, class Hash = std::hash<Key>,
          class KeyEqual = std::equal_to<Key>,
          class Allocator = AlignedAllocator<std::pair<const Key, T>>>
using HashMap = std::unordered_map<Key, T, Hash, KeyEqual, Allocator>;
#else
static_assert(false, "Ambigious allocator");
#endif
}
#endif // !APPLICATION_API_DATASTRUCTURES_CONTAINERS_HASHMAP_HPP_
