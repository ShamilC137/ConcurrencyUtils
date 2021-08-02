#include "SlotDetails.hpp"

namespace impl {
BaseSlot::BaseSlot(const int *idseq, const int *retid, const int priority) noexcept
    : idseq_ptr_{idseq}, retid_ptr_{retid}, priority_{priority} {}

void BaseSlot::operator()(BaseTask *task) noexcept(false) { 
  RealCall(task); 
}
} // namespace impl
