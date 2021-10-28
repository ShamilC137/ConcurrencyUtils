#include "DeferThreadWrapper.hpp"

namespace api {
DeferThreadWrapper::DeferThreadWrapper() noexcept : thread_{} {}

DeferThreadWrapper::DeferThreadWrapper(DeferThread *thread) noexcept
    : thread_{thread} {}

DeferThreadWrapper::DeferThreadWrapper(const DeferThreadWrapper &rhs) noexcept
    : thread_{rhs.thread_} {}

DeferThreadWrapper::DeferThreadWrapper(DeferThreadWrapper &&rhs) noexcept
    : thread_{rhs.thread_} {
  rhs.thread_ = nullptr;
}

DeferThreadWrapper &
DeferThreadWrapper::operator=(const DeferThreadWrapper &rhs) noexcept {
  thread_ = rhs.thread_;
  return *this;
}

DeferThreadWrapper &
DeferThreadWrapper::operator=(DeferThreadWrapper &&rhs) noexcept {
  thread_ = rhs.thread_;
  rhs.thread_ = nullptr;
  return *this;
}

void DeferThreadWrapper::Detach() { thread_->Detach(); }

void DeferThreadWrapper::Join() { thread_->Join(); }

auto DeferThreadWrapper::NativeHandle() { return thread_->GetNativeHandle(); }

[[nodiscard]] bool DeferThreadWrapper::Joinable() const noexcept {
  return thread_->Joinable();
}

[[nodiscard]] unsigned int DeferThreadWrapper::HardwareConcurrency() noexcept {
  return thread_->HardwareConcurrency();
}

[[nodiscard]] auto DeferThreadWrapper::GetId() const noexcept {
  return thread_->GetId();
}
} // namespace api
