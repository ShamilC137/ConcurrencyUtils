#ifndef APPLICATION_API_CONTAINERS_DEQUE_HPP_
#define APPLICATION_API_CONTAINERS_DEQUE_HPP_
#include "../Memory/VAllocator.hpp"

#include <deque>

namespace api {
template <class T, class Allocator = std::allocator<T>/*api::VAllocator<T>*/>
using Deque = std::deque<T, Allocator>;
}
#endif // !APPLICATION_KERNEL_API_CONTAINERS_DEQUE_HPP_
