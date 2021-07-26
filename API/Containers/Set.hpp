#ifndef APPLICATION_API_CONTAINERS_SET_HPP_
#define APPLICATION_API_CONTAINERS_SET_HPP_
#include "../Memory/VAllocator.hpp"
#include <set>

namespace api {
template <class Key, class Compare = std::less<Key>,
          class Allocator = std::allocator<Key>/*api::VAllocator<Key>*/>
using Set = std::set<Key, Compare, Allocator>;
}
#endif // !APPLICATION_API_CONTAINERS_SET_HPP_
