#include "ScopedSlotWrapper.hpp"

namespace api {
namespace kernel_api {
void Deallocate(void *ptr, const std::size_t nbytes) noexcept;
}

ScopedSlotWrapper::ScopedSlotWrapper(const PointerType &slot) noexcept
    : slot_{slot} {}

ScopedSlotWrapper::ScopedSlotWrapper(ScopedSlotWrapper &&rhs) noexcept
    : slot_{rhs.slot_} {
  rhs.slot_ = nullptr;
}

ScopedSlotWrapper &ScopedSlotWrapper::operator=(
    ScopedSlotWrapper &&rhs) noexcept {
  this->~ScopedSlotWrapper(); // clear current context
  new (this) ScopedSlotWrapper(std::move(rhs));
  return *this;
}

ScopedSlotWrapper::~ScopedSlotWrapper() noexcept {
  if (!slot_) {
    return;
  }
  const auto mysize{slot_->SizeInBytes()};
  slot_->~BaseSlot();
#if ALIGNED_ALLOCATOR_USAGE
  kernel_api::Deallocate(slot_, mysize);
#elif STL_ALLOCATOR_USAGE
  ::operator delete(slot_, mysize);
#endif
}
} // namespace api
