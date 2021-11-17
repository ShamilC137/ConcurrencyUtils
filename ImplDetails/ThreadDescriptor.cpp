#include "ThreadDescriptor.hpp"

namespace impl {
ThreadDescriptor::ThreadDescriptor(api::DeferThread *thread) noexcept
    : thread_{thread}, deleted_{} {}

void ThreadDescriptor::DecrementNumberOfReferences() noexcept {
  nreferences_.sub(1u, api::MemoryOrder::relaxed);
}

bool ThreadDescriptor::IncrementNumberOfReferences() noexcept {
  api::ScopedLock<api::Mutex> lock(mutex_);
  if (deleted_) {
    return false;
  } else {
    return nreferences_.add(1u, api::MemoryOrder::relaxed);
  }
}

[[nodiscard]] api::DeferThread *ThreadDescriptor::GetThread() noexcept {
  return thread_;
}

[[nodiscard]] bool ThreadDescriptor::IsDeleted() const noexcept {
  return deleted_;
}

[[nodiscard]] bool ThreadDescriptor::MarkAsDeleted() noexcept {
  api::ScopedLock<api::Mutex> lock(mutex_);
  deleted_ = true;
  return nreferences_.load(api::MemoryOrder::relaxed) == 0u;
}

[[nodiscard]] volatile api::ThreadSignals &
ThreadDescriptor::GetSignals() volatile noexcept {
  return signals_;
}
} // namespace impl
