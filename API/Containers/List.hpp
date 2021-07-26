#ifndef APPLICATION_API_CONTAINERS_LIST_HPP_
#define APPLICATION_API_CONTAINERS_LIST_HPP_
#include "../Memory/VAllocator.hpp"
#include <list>

namespace api {
template <class T, class Allocator = /*api::VAllocator<T>*/std::allocator<T>>
using List = std::list<T, Allocator>;
}
#endif // !APPLICATION_API_CONTAINERS_LIST_HPP_
