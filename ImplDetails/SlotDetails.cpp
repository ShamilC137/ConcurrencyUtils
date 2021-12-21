#include "SlotDetails.hpp"

namespace impl {
BaseSlot::BaseSlot(const int *idseq, const int *retid) noexcept
    : idseq_ptr_{idseq}, retid_ptr_{retid} {}

void BaseSlot::SetPriority(const api::String &signal,
                           const int priority) noexcept {
  assert(priority != 0 && priority >= -1);
  api::ScopedLock<api::SharedMutex> lock(priorities_mutex_);
  priorities_[signal] = priority;
}

int BaseSlot::GetPriority(const api::String &signal) const noexcept(false) {
  api::SharedLockGuard<api::SharedMutex> lock(priorities_mutex_);
  return priorities_.at(signal);
}

void BaseSlot::operator()(api::TaskWrapper &task) const noexcept(false) {
  RealCall(task);
}
void BaseSlot::operator()(api::TaskWrapper &&task) const noexcept(false) {
  RealCall(task);  // lvalue ref here
}
}  // namespace impl
