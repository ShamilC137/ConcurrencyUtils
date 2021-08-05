#ifndef APPLICATION_API_DATASTRUCTURES_CONTAINERS_LIST_HPP_
#define APPLICATION_API_DATASTRUCTURES_CONTAINERS_LIST_HPP_
// current project
#include "../../../Config.hpp"
#if ALIGNED_ALLOCATOR_USAGE
#include "../../Memory/AlignedAllocator.hpp"
#endif // !ALIGNED_ALLOCATOR_USAGE

// STL
#include <list>

namespace api {
#if STL_ALLOCATOR_USAGE
template <class T, class Allocator = std::allocator<T>>
using List = std::list<T, Allocator>;
#elif ALIGNED_ALLOCATOR_USAGE
template <class T, class Allocator = AlignedAllocator<T>>
using List = std::list<T, Allocator>;
#else
static_assert(false, "Ambigious allocator");
#endif
}
#endif // !APPLICATION_API_DATASTRUCTURES_CONTAINERS_LIST_HPP_
