#ifndef APPLICATION_API_DATASTRUCTURES_CONTAINERS_SET_HPP_
#define APPLICATION_API_DATASTRUCTURES_CONTAINERS_SET_HPP_

// current project
#include "../../../Config.hpp"
#include "../../Memory/VAllocator.hpp"

// STL
#include <set>

namespace api {
#if STL_ALLOCATOR_USAGE
template <class Key, class Compare = std::less<Key>,
          class Allocator = std::allocator<Key>>
using Set = std::set<Key, Compare, Allocator>;
#elif VALLOCATOR_USAGE
template <class Key, class Compare = std::less<Key>,
          class Allocator = api::VAllocator<Key>>
using Set = std::set<Key, Compare, Allocator>;
#else
static_assert(false, "Ambigious allocator");
#endif
} // namespace api
#endif // !APPLICATION_API_DATASTRUCTURES_CONTAINERS_SET_HPP_
