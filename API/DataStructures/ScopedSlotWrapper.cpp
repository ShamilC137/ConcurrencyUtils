#include "ScopedSlotWrapper.hpp"

namespace api {
namespace kernel_api {
void Deallocate(void *ptr, const std::size_t nbytes) noexcept;
}

ScopedSlotWrapper::ScopedSlotWrapper(const PointerType &slot) noexcept
    : slot_{slot} {}

ScopedSlotWrapper::~ScopedSlotWrapper() noexcept {
  const auto mysize{slot_->SizeInBytes()};
  slot_->~BaseSlot();
#if ALIGNED_ALLOCATOR_USAGE
  kernel_api::Deallocate(slot_, mysize);
#elif STL_ALLOCATOR_USAGE
  ::operator delete(slot_, mysize);
#endif
}
} // namespace api
