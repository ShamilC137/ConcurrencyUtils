#ifndef APPLICATION_API_DATASTRUCTURES_CONTAINERS_VECTOR_HPP_
#define APPLICATION_API_DATASTRUCTURES_CONTAINERS_VECTOR_HPP_

// STL
#include <vector>

namespace api {
template <class T, class Allocator = std::allocator<T>>
using Vector = std::vector<T, Allocator>;
} // namespace api

#endif // !APPLICATION_API_DATASTRUCTURES_CONTAINERS_VECTOR_HPP_
