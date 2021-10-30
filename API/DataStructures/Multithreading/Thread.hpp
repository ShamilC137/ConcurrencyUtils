#ifndef APPLICATION_API_DATASTRUCTURES_MULTITHREADING_THREAD_HPP_
#define APPLICATION_API_DATASTRUCTURES_MULTITHREADING_THREAD_HPP_

// STL
#include <thread>

namespace api {
using Thread = std::thread;

// Returns hashed caller thread id
[[nodiscard]] inline std::size_t GetId() noexcept {
  return std::hash<Thread::id>{}.operator()(std::this_thread::get_id());
}
} // namespace api
#endif
