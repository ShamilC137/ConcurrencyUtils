#ifndef APPLICATION_API_DATASTRUCTURES_CONTAINERS_SET_HPP_
#define APPLICATION_API_DATASTRUCTURES_CONTAINERS_SET_HPP_

// STL
#include <set>

namespace api {
template <class Key, class Compare = std::less<Key>,
          class Allocator = std::allocator<Key>>
using Set = std::set<Key, Compare, Allocator>;
} // namespace api
#endif // !APPLICATION_API_DATASTRUCTURES_CONTAINERS_SET_HPP_
