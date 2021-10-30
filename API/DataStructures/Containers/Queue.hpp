#ifndef APPLICATION_API_DATASTRUCTURES_CONTAINERS_QUEUE_HPP_
#define APPLICATION_API_DATASTRUCTURES_CONTAINERS_QUEUE_HPP_
#include "Deque.hpp"

// STL
#include <queue>

namespace api {
template <class Value, class Container = Deque<Value>>
using Queue = std::queue<Value, Container>;
}
#endif // !APPLICATION_API_DATASTRUCTURES_CONTAINERS_QUEUE_HPP_
