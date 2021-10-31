#ifndef APPLICATION_API_DATASTRUCTURES_MULTITHREADING_THREAD_HPP_
#define APPLICATION_API_DATASTRUCTURES_MULTITHREADING_THREAD_HPP_

// STL
#include <thread>

namespace api {
using Thread = std::thread;

// Returns hashed given id
[[nodiscard]] std::size_t GetHashedId(const Thread::id &id) noexcept;

// Returns hashed caller thread id
[[nodiscard]] std::size_t GetHashedId() noexcept;
} // namespace api
#endif
