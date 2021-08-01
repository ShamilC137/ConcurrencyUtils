#include "SlotDetails.hpp"

namespace impl {
BaseSlot::BaseSlot(const int *idseq, const int *retid, const int order) noexcept
    : idseq_ptr_{idseq}, retid_ptr_{retid}, order_{order} {}

void BaseSlot::operator()(BaseTask *task) noexcept(false) { 
  RealCall(task); 
}
} // namespace impl
