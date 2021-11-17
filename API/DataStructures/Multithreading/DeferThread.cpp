#include "DeferThread.hpp"

namespace api {
[[nodiscard]] Thread &DeferThread::GetAttachedThread() noexcept {
  return thread_;
}

[[nodiscard]] const Thread &DeferThread::GetAttachedThread() const noexcept {
  return thread_;
}

[[nodiscard]] AtomicFlag &DeferThread::GetIsActiveFlag() noexcept {
  return is_active_;
}

[[nodiscard]] const AtomicFlag &DeferThread::GetIsActiveFlag() const noexcept {
  return is_active_;
}

void DeferThread::ActivateThread(api::MemoryOrder order) {
  is_active_.test_and_set(order);
  is_active_.notify_one();
}

void DeferThread::DeactivateThread(api::MemoryOrder order) {
  is_active_.clear();
  is_active_.wait(false, order);
}

void DeferThread::Join() { thread_.join(); }

void DeferThread::Detach() { thread_.detach(); }

[[nodiscard]] DeferThread::NativeHandle DeferThread::GetNativeHandle() {
  return thread_.native_handle();
}

[[nodiscard]] bool DeferThread::Joinable() const noexcept {
  return thread_.joinable();
}

[[nodiscard]] unsigned int DeferThread::HardwareConcurrency() noexcept {
  return thread_.hardware_concurrency();
}

[[nodiscard]] DeferThread::ID DeferThread::GetId() const noexcept {
  return thread_.get_id();
}
} // namespace api
