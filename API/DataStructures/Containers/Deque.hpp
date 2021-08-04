#ifndef APPLICATION_API_DATASTRUCTURES_CONTAINERS_DEQUE_HPP_
#define APPLICATION_API_DATASTRUCTURES_CONTAINERS_DEQUE_HPP_
// current project
#include "../../../Config.hpp"
#include "../../Memory/AlignedAllocator.hpp"

// STL
#include <deque>

namespace api {
#if STL_ALLOCATOR_USAGE
template <class T, class Allocator = std::allocator<T>>
using Deque = std::deque<T, Allocator>;
#elif ALIGNED_ALLOCATOR_USAGE
template <class T, class Allocator = api::AlignedAllocator<T>>
using Deque = std::deque<T, Allocator>;
#else // if some allocator not specified
static_assert(false, "Ambigious allocator");
#endif
}
#endif // !APPLICATION_KERNEL_API_DATASTRUCTURES_CONTAINERS_DEQUE_HPP_
