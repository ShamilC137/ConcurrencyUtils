#ifndef APPLICATION_API_DATASTRUCTURES_CONTAINERS_VECTOR_HPP_
#define APPLICATION_API_DATASTRUCTURES_CONTAINERS_VECTOR_HPP_
// current project
#include "../../../Config.hpp"
#include "../../Memory/VAllocator.hpp"

// STL
#include <vector>

namespace api {
#if STL_ALLOCATOR_USAGE
template <class T, class Allocator = std::allocator<T>>
using Vector = std::vector<T, Allocator>;
#elif VALLOCATOR_USAGE
template <class T, class Allocator = api::VAllocator<T>>
using Vector = std::vector<T, Allocator>;
#else
static_assert(false, "Ambigious allocator");
#endif
} // namespace api

#endif // !APPLICATION_API_DATASTRUCTURES_CONTAINERS_VECTOR_HPP_
