#ifndef APPLICATION_API_DATASTRUCTURES_CONTAINERS_DEQUE_HPP_
#define APPLICATION_API_DATASTRUCTURES_CONTAINERS_DEQUE_HPP_

// STL
#include <deque>

namespace api {
template <class T, class Allocator = std::allocator<T>>
using Deque = std::deque<T, Allocator>;
}
#endif // !APPLICATION_KERNEL_API_DATASTRUCTURES_CONTAINERS_DEQUE_HPP_
