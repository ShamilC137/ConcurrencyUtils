#ifndef APPLICATION_API_DATASTRUCTURES_MULTITHREADING_THREAD_HPP_
#define APPLICATION_API_DATASTRUCTURES_MULTITHREADING_THREAD_HPP_

// STL
#include <thread>

namespace api {
using Thread = std::thread;
using ThreadId = std::thread::id;

// Returns hashed caller thread id
[[nodiscard]] ThreadId GetId() noexcept;
} // namespace api
#endif
