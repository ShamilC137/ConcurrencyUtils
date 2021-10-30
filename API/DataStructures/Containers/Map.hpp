#ifndef APPLICATION_API_DATASTRUCTURES_CONTAINERS_MAP_HPP_
#define APPLICATION_API_DATASTRUCTURES_CONTAINERS_MAP_HPP_

// STL
#include <map>

namespace api {
template <class Key, class Value, class Compare = std::less<Key>,
          class Allocator = std::allocator<std::pair<const Key, Value>>>
using Map = std::map<Key, Value, Compare, Allocator>;
} // namespace api
#endif // !APPLICATION_API_DATASTRUCTURES_CONTAINERS_MAP_HPP_
