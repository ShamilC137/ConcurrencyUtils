#include "TaskDetails.hpp"

namespace impl {
// ctor and dtor
BaseTask::BaseTask(const api::String &signal_sig, const bool is_blocking_task,
                   api::TaskPriority priority, const int *idseq,
                   const int *retid) noexcept
    : signal_sig_{signal_sig},
      is_blocking_task_{is_blocking_task}, priority_{priority},
      idseq_ptr_{idseq}, retid_ptr_{retid}, nreferences_{}, nacceptors_{} {}

BaseTask::~BaseTask() noexcept {
  assert(nreferences_.load(api::MemoryOrder::acquire) == 0u &&
         "Deleting alive task");
}

// modifiers
void BaseTask::NotifyAboutComplete() noexcept {
  nacceptors_.sub(1u, api::MemoryOrder::release);
  if (!IsWaitable()) {
    ClearArguments();
  }
  nacceptors_.notify_all();
}

// sync operations
void BaseTask::Wait(const unsigned char expected_value) noexcept(false) {
  auto old{nacceptors_.load(api::MemoryOrder::acquire)};
  if (expected_value > old) {
    throw api::Deadlock("Unreachible expected value");
  }

  for (; old != expected_value;
       old = nacceptors_.load(api::MemoryOrder::acquire)) {
    nacceptors_.wait(old, api::MemoryOrder::acquire);
  }
}
} // namespace impl
