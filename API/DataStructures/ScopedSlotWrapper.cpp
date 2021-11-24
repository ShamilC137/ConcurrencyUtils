#include "ScopedSlotWrapper.hpp"

namespace api {
ScopedSlotWrapper::ScopedSlotWrapper(const PointerType &slot) noexcept
    : slot_{slot} {}

ScopedSlotWrapper::ScopedSlotWrapper(ScopedSlotWrapper &&rhs) noexcept
    : slot_{rhs.slot_} {
  rhs.slot_ = nullptr;
}

ScopedSlotWrapper &ScopedSlotWrapper::operator=(
    ScopedSlotWrapper &&rhs) noexcept {
  this->~ScopedSlotWrapper();  // clear current context
  new (this) ScopedSlotWrapper(std::move(rhs));
  return *this;
}

ScopedSlotWrapper::~ScopedSlotWrapper() noexcept {
  if (slot_) {
    delete slot_;
  }
}
}  // namespace api
