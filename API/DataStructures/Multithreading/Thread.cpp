#include "Thread.hpp"

namespace api {
// Returns hashed caller thread id
[[nodiscard]] ThreadId GetId() noexcept {
  return std::this_thread::get_id();
}
} // namespace api
