#include "TaskDetails.hpp"

namespace impl {
// ctor and dtor
BaseTask::BaseTask(const api::String &module_id, const bool is_blocking_task,
                   const int *idseq, const int *retid) noexcept
    : mid_{module_id}, is_blocking_task_{is_blocking_task}, idseq_ptr_{idseq},
      retid_ptr_{retid}, nreferences_{}, nacceptors_{} {}

BaseTask::~BaseTask() noexcept(false) {
  if (nreferences_ != 0u) {
    throw api::AliveTaskDeletion("Deleting task with references");
  }
}

// modifiers
void BaseTask::NotifyAboutComplete() {
  nacceptors_.sub(1, api::MemoryOrder::acq_rel);
  assert(nacceptors_.load(api::MemoryOrder::relaxed) != 255);
  nacceptors_.notify_all();
}

// sync operations
void BaseTask::Wait(const unsigned char expected_value) noexcept(false) {
  auto old{nacceptors_.load(api::MemoryOrder::acquire)};

  if (expected_value > old) {
    throw api::Deadlock("Unreachible expected value");
  }

  for (; old != expected_value;
       old = nacceptors_.load(api::MemoryOrder::relaxed)) {
    nacceptors_.wait(old, api::MemoryOrder::relaxed);
  }
}
} // namespace impl
