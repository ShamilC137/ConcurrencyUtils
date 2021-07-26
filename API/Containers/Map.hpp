#ifndef APPLICATION_API_CONTAINERS_MAP_HPP_
#define APPLICATION_API_CONTAINERS_MAP_HPP_
#include "../Memory/VAllocator.hpp"
#include <map>

namespace api {
template <
    class Key, class Value, class Compare = std::less<Key>,
    class Allocator = std::allocator<std::pair<
        const Key, Value>> /*api::VAllocator<std::pair<const Key, Value>>*/>
using Map = std::map<Key, Value, Compare, Allocator>;
}
#endif // !APPLICATION_API_CONTAINERS_MAP_HPP_
