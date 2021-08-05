#ifndef APPLICATION_API_DATASTRUCTURES_CONTAINERS_VECTOR_HPP_
#define APPLICATION_API_DATASTRUCTURES_CONTAINERS_VECTOR_HPP_
// current project
#include "../../../Config.hpp"
#if ALIGNED_ALLOCATOR_USAGE
#include "../../Memory/AlignedAllocator.hpp"
#endif // !ALIGNED_ALLOCATOR_USAGE

// STL
#include <vector>

namespace api {
#if STL_ALLOCATOR_USAGE
template <class T, class Allocator = std::allocator<T>>
using Vector = std::vector<T, Allocator>;
#elif	ALIGNED_ALLOCATOR_USAGE
template <class T, class Allocator = AlignedAllocator<T>>
using Vector = std::vector<T, Allocator>;
#else
static_assert(false, "Ambigious allocator");
#endif
} // namespace api

#endif // !APPLICATION_API_DATASTRUCTURES_CONTAINERS_VECTOR_HPP_
