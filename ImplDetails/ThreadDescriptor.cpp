#include "ThreadDescriptor.hpp"

namespace impl {
ThreadDescriptor::ThreadDescriptor(api::DeferThread *thread) noexcept
    : thread_{thread} {}

void ThreadDescriptor::DecrementNumberOfReferences() noexcept {
  nreferences_.sub(1u, api::MemoryOrder::acq_rel);
}

bool ThreadDescriptor::IncrementNumberOfReferences() noexcept {
  api::ScopedLock<api::Mutex> lock(mutex_);
  if (will_be_deleted_) {
    return false;
  } else {
    return nreferences_.add(1u, api::MemoryOrder::acq_rel);
  }
}

[[nodiscard]] api::DeferThread *ThreadDescriptor::GetThread() noexcept {
  return thread_;
}

[[nodiscard]] const volatile bool &
ThreadDescriptor::ShallBeDeleted() const noexcept {
  return will_be_deleted_;
}

[[nodiscard]] bool ThreadDescriptor::CouldBeDeleted() noexcept {
  api::ScopedLock<api::Mutex> lock(mutex_);
  will_be_deleted_ = true;
  if (nreferences_.load(api::MemoryOrder::acquire) == 0u) {
    return true;
  } else {
    return false;
  }
}

[[nodiscard]] volatile api::ThreadSignals &
ThreadDescriptor::GetSignals() volatile noexcept {
  return signals_;
}
} // namespace impl
