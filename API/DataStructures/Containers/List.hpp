#ifndef APPLICATION_API_DATASTRUCTURES_CONTAINERS_LIST_HPP_
#define APPLICATION_API_DATASTRUCTURES_CONTAINERS_LIST_HPP_

// STL
#include <list>

namespace api {
template <class T, class Allocator = std::allocator<T>>
using List = std::list<T, Allocator>;
}
#endif // !APPLICATION_API_DATASTRUCTURES_CONTAINERS_LIST_HPP_
