#ifndef APPLICATION_API_DATASTRUCTURES_CONTAINERS_HASHSET_HPP_
#define APPLICATION_API_DATASTRUCTURES_CONTAINERS_HASHSET_HPP_
// current project
#include "../../../Config.hpp"
#include "../../Memory/AlignedAllocator.hpp"

// STL
#include <unordered_set>

namespace api {
#if STL_ALLOCATOR_USAGE
template <class Key, class Hash = std::hash<Key>,
          class Comparator = std::equal_to<Key>,
          class Allocator = std::allocator<Key>>
using HashSet = std::unordered_set<Key, Hash, Comparator, Allocator>;
#elif ALIGNED_ALLOCATOR_USAGE
template <class Key, class Hash = std::hash<Key>,
          class Comparator = std::equal_to<Key>,
          class Allocator = AlignedAllocator<Key>>
using HashSet = std::unordered_set<Key, Hash, Comparator, Allocator>;
#else
static_assert(false, "Ambigious allocator");
#endif
}
#endif // !APPLICATION_API_DATASTRUCTURES_CONTAINERS_HASHSET_HPP_
