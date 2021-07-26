#ifndef APPLICATION_API_CONTAINERS_VECTOR_HPP_
#define APPLICATION_API_CONTAINERS_VECTOR_HPP_
#include "../Memory/VAllocator.hpp"
#include <vector>

namespace api {
template <class T, class Allocator = /*api::VAllocator<T>*/std::allocator<T>>
using Vector = std::vector<T, Allocator>;
}

#endif // !APPLICATION_API_CONTAINERS_VECTOR_HPP_
