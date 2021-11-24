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

void DeferThread::Activate(api::MemoryOrder order) {
  is_active_.test_and_set(order);
  is_active_.notify_one();
}

void DeferThread::DeactivateCallerThread(api::MemoryOrder order) {
  is_active_.clear();
  is_active_.wait(false, order);
}

void DeferThread::Join() {
  Close();
  thread_.join();
}

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

[[nodiscard]] const volatile ThreadSignals &DeferThread::GetSignals() const
    volatile noexcept {
  return signals_;
}

void DeferThread::SetSignal(ThreadSignal signal) volatile noexcept {
  signals_.Set(signal);
}

void DeferThread::UnsetSignal(ThreadSignal signal) volatile noexcept {
  signals_.Unset(signal);
}

void DeferThread::Close() noexcept {
  ScopedLock<Mutex> lock(close_mutex_);
  if (!is_closed_) {
    signals_.Set(ThreadSignal::kExit);
    is_closed_ = true;
  }
}

[[nodiscard]] bool DeferThread::IsClosed() const volatile noexcept {
  return is_closed_;
}

[[nodiscard]] unsigned char DeferThread::IncrementNumberOfReferences(
    api::MemoryOrder order) noexcept {
  ScopedLock<Mutex> lock(close_mutex_);
  if (is_closed_) {
    return 0;
  } else {
    return nreferences_.add(1, order);
  }
}

unsigned char DeferThread::DecrementNumberOfReferences(
    api::MemoryOrder order) noexcept {
  return nreferences_.sub(1, order);
}

[[nodiscard]] unsigned char DeferThread::NumberOfReferences(
    api::MemoryOrder order) const noexcept {
  return nreferences_.load(order);
}
}  // namespace api
