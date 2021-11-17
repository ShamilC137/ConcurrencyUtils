#include "DeferThreadWrapper.hpp"

namespace api {
DeferThreadWrapper::DeferThreadWrapper() noexcept : desc_{} {}

DeferThreadWrapper::~DeferThreadWrapper() noexcept {
  if (desc_) {
    desc_->DecrementNumberOfReferences();
  }
}

DeferThreadWrapper::DeferThreadWrapper(impl::ThreadDescriptor *desc) noexcept
    : desc_{desc} {
  if (desc_ && !desc_->IncrementNumberOfReferences()) {
    desc_ = nullptr;
  }
}

DeferThreadWrapper::DeferThreadWrapper(const DeferThreadWrapper &rhs) noexcept
    : desc_{rhs.desc_} {
  if (desc_) {
    if (!desc_->IncrementNumberOfReferences()) {
      desc_ = nullptr;
    }
  }
}

DeferThreadWrapper::DeferThreadWrapper(DeferThreadWrapper &&rhs) noexcept
    : desc_{rhs.desc_} {
  rhs.desc_ = nullptr;
}

DeferThreadWrapper &
DeferThreadWrapper::operator=(const DeferThreadWrapper &rhs) noexcept {
  desc_ = rhs.desc_;
  if (desc_) {
    if (!desc_->IncrementNumberOfReferences()) {
      desc_ = nullptr;
    }
  }
  return *this;
}

DeferThreadWrapper &
DeferThreadWrapper::operator=(DeferThreadWrapper &&rhs) noexcept {
  desc_ = rhs.desc_;
  rhs.desc_ = nullptr;
  return *this;
}

void DeferThreadWrapper::Detach() {
  if (desc_) {
    if (desc_->IsDeleted()) {
      desc_->DecrementNumberOfReferences();
      desc_ = nullptr;
    } else {
      desc_->GetThread()->Detach();
    }
  }
}

void DeferThreadWrapper::Join() {
  if (desc_) {
    if (desc_->IsDeleted()) {
      desc_->DecrementNumberOfReferences();
      desc_ = nullptr;
    } else {
      desc_->GetThread()->Join();
    }
  }
}

auto DeferThreadWrapper::NativeHandle() noexcept(false) {
  if (desc_) {
    if (desc_->IsDeleted()) {
      desc_->DecrementNumberOfReferences();
      desc_ = nullptr;
    } else {
      return desc_->GetThread()->GetNativeHandle();
    }
  }
  throw ExpiredThread("Thread is deleted");
}

[[nodiscard]] bool DeferThreadWrapper::Joinable() noexcept(false) {
  if (desc_) {
    if (desc_->IsDeleted()) {
      desc_->DecrementNumberOfReferences();
      desc_ = nullptr;
    } else {
      return desc_->GetThread()->Joinable();
    }
  }
  throw ExpiredThread("Thread is deleted");
}

[[nodiscard]] unsigned int
DeferThreadWrapper::HardwareConcurrency() noexcept(false) {
  if (desc_) {
    if (desc_->IsDeleted()) {
      desc_->DecrementNumberOfReferences();
      desc_ = nullptr;
    } else {
      return desc_->GetThread()->HardwareConcurrency();
    }
  }
  throw ExpiredThread("Thread is deleted");
}

[[nodiscard]] auto DeferThreadWrapper::GetId() noexcept(false) {
  if (desc_) {
    if (desc_->IsDeleted()) {
      desc_->DecrementNumberOfReferences();
      desc_ = nullptr;
    } else {
      return desc_->GetThread()->GetId();
    }
  }
  throw ExpiredThread("Thread is deleted");
}

void DeferThreadWrapper::SendSuspendRequest() noexcept {
  try {
    kernel_api::SendSuspendThreadSignal(api::GetId(*this));
  } catch (...) {
    return;
  }
}

bool DeferThreadWrapper::Resume() noexcept(false) {
  try {
    return kernel_api::ResumeThread(api::GetId(*this));
  } catch (...) {
    return false;
  }
}

[[nodiscard]] ThreadId GetId(DeferThreadWrapper &thread) noexcept {
  return thread.GetId();
}
} // namespace api
