#include "SlotDetails.hpp"

namespace impl {
BaseSlot::BaseSlot(const int *idseq) noexcept : idseq_ptr_{idseq} {}

void BaseSlot::operator()(BaseTask *task) noexcept(false) { RealCall(task); }
}  // namespace impl