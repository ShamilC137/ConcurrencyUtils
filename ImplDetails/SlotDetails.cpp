#include "SlotDetails.hpp"

namespace impl {
BaseSlot::BaseSlot(const int *idseq, const int *retid,
                   const int priority) noexcept
    : idseq_ptr_{idseq}, retid_ptr_{retid}, priority_{priority} {}

void BaseSlot::operator()(api::TaskWrapper &task) noexcept(false) {
  RealCall(task);
}
void BaseSlot::operator()(api::TaskWrapper &&task) noexcept(false) {
  RealCall(task); // lvalue ref here
}
} // namespace impl

namespace api {
SlotWrapper::SlotWrapper(const PointerType &slot,
                         const Allocator &alloc) noexcept
    : slot{slot}, alloc{alloc} {}

SlotWrapper::~SlotWrapper() noexcept {
  const auto mysize{slot->SizeInBytes()};
  std::allocator_traits<Allocator>::destroy(alloc, slot);
  std::allocator_traits<Allocator>::deallocate(alloc, slot, mysize);
}
} // namespace api
