#include "Thread.hpp"

namespace api {
[[nodiscard]] std::size_t GetHashedId(const Thread::id &id) noexcept {
  return std::hash<Thread::id>{}.operator()(id);
}

// Returns hashed caller thread id
[[nodiscard]] std::size_t GetHashedId() noexcept {
  return GetHashedId(std::this_thread::get_id());
}
} // namespace api
