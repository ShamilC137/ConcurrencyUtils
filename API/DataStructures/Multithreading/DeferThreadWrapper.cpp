#include "DeferThreadWrapper.hpp"

namespace api {
DeferThreadWrapper::DeferThreadWrapper() noexcept : thread_{} {}

DeferThreadWrapper::~DeferThreadWrapper() noexcept {
  if (thread_) {
    thread_->DecrementNumberOfReferences();
  }
}

DeferThreadWrapper::DeferThreadWrapper(DeferThread *thread) noexcept
    : thread_{thread} {
  if (thread_ && !thread_->IncrementNumberOfReferences()) {
    thread_ = nullptr;
  }
}

DeferThreadWrapper::DeferThreadWrapper(const DeferThreadWrapper &rhs) noexcept
    : thread_{rhs.thread_} {
  if (thread_) {
    if (!thread_->IncrementNumberOfReferences()) {
      thread_ = nullptr;
    }
  }
}

DeferThreadWrapper::DeferThreadWrapper(DeferThreadWrapper &&rhs) noexcept
    : thread_{rhs.thread_} {
  rhs.thread_ = nullptr;
}

DeferThreadWrapper &DeferThreadWrapper::operator=(
    const DeferThreadWrapper &rhs) noexcept {
  thread_ = rhs.thread_;
  if (thread_) {
    if (!thread_->IncrementNumberOfReferences()) {
      thread_ = nullptr;
    }
  }
  return *this;
}

DeferThreadWrapper &DeferThreadWrapper::operator=(
    DeferThreadWrapper &&rhs) noexcept {
  thread_ = rhs.thread_;
  rhs.thread_ = nullptr;
  return *this;
}

void DeferThreadWrapper::Detach() {
  if (thread_) {
    if (thread_->IsClosed()) {
      thread_->DecrementNumberOfReferences();
      thread_ = nullptr;
    } else {
      thread_->Detach();
    }
  }
}

void DeferThreadWrapper::Join() {
  if (thread_) {
    if (thread_->IsClosed()) {
      thread_->DecrementNumberOfReferences();
      thread_ = nullptr;
    } else {
      thread_->Close();
      thread_->Join();
    }
  }
}

auto DeferThreadWrapper::NativeHandle() noexcept(false) {
  if (thread_) {
    if (thread_->IsClosed()) {
      thread_->DecrementNumberOfReferences();
      thread_ = nullptr;
    } else {
      return thread_->GetNativeHandle();
    }
  }
  throw ExpiredThread("Thread is closed");
}

[[nodiscard]] bool DeferThreadWrapper::Joinable() noexcept(false) {
  if (thread_) {
    if (thread_->IsClosed()) {
      thread_->DecrementNumberOfReferences();
      thread_ = nullptr;
    } else {
      return thread_->Joinable();
    }
  }
  throw ExpiredThread("Thread is deleted");
}

[[nodiscard]] unsigned int DeferThreadWrapper::HardwareConcurrency() noexcept(
    false) {
  if (thread_) {
    if (thread_->IsClosed()) {
      thread_->DecrementNumberOfReferences();
      thread_ = nullptr;
    } else {
      return thread_->HardwareConcurrency();
    }
  }
  throw ExpiredThread("Thread is deleted");
}

[[nodiscard]] DeferThread::ID DeferThreadWrapper::GetId() noexcept(false) {
  if (thread_) {
    if (thread_->IsClosed()) {
      thread_->DecrementNumberOfReferences();
      thread_ = nullptr;
    } else {
      return thread_->GetId();
    }
  }
  throw ExpiredThread("Thread is deleted");
}

void DeferThreadWrapper::Start() noexcept { thread_->Activate(); }

bool DeferThreadWrapper::IsClosed() const noexcept {
  if (thread_) {
    return thread_->IsClosed();
  } else {
    return true;
  }
}

[[nodiscard]] ThreadId GetId(DeferThreadWrapper &thread) noexcept {
  return thread.GetId();
}
}  // namespace api
