#ifndef APPLICATION_API_CONTAINERS_QUEUE_HPP_
#define APPLICATION_API_CONTAINERS_QUEUE_HPP_
#include "../Memory/VAllocator.hpp"
#include "Deque.hpp"
#include <queue>

namespace api {
template <class Value, class Container = Deque<Value>>
using Queue = std::queue<Value, Container>;
}
#endif // !APPLICATION_API_CONTAINERS_QUEUE_HPP_
