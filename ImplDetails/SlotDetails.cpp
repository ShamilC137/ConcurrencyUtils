#include "SlotDetails.hpp"

namespace impl {
BaseSlot::BaseSlot(const int *idseq, const int *retid) noexcept
    : idseq_ptr_{idseq}, retid_ptr_{retid} {}

void BaseSlot::operator()(api::TaskWrapper &task) const noexcept(false) {
  RealCall(task);
}
void BaseSlot::operator()(api::TaskWrapper &&task) const noexcept(false) {
  RealCall(task);  // lvalue ref here
}
}  // namespace impl
