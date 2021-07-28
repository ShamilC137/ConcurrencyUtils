#ifndef APPLICATION_API_DATASTRUCTURES_CONTAINERS_MAP_HPP_
#define APPLICATION_API_DATASTRUCTURES_CONTAINERS_MAP_HPP_
// current project
#include "../../../Config.hpp"
#include "../../Memory/VAllocator.hpp"

// STL
#include <map>

namespace api {
#if STL_ALLOCATOR_USAGE
template <class Key, class Value, class Compare = std::less<Key>,
          class Allocator = std::allocator<std::pair<const Key, Value>>>
using Map = std::map<Key, Value, Compare, Allocator>;
#elif VALLOCATOR_USAGE
template <class Key, class Value, class Compare = std::less<Key>,
          class Allocator = api::VAllocator<std::pair<const Key, Value>>>
using Map = std::map<Key, Value, Compare, Allocator>;
#else
static_assert(false, "Ambigious allocator");
#endif
} // namespace api
#endif // !APPLICATION_API_DATASTRUCTURES_CONTAINERS_MAP_HPP_
