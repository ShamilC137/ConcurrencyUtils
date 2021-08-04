#include "TaskDetails.hpp"

namespace impl {
// ctor and dtor
BaseTask::BaseTask(const api::String &module_id, const bool is_blocking_task,
                   const int *idseq, const int *retid) noexcept
    : mid_{module_id}, is_blocking_task_{is_blocking_task}, idseq_ptr_{idseq},
      retid_ptr_{retid}, nreferences_{},
      nacceptors_(static_cast<unsigned char>(
          is_blocking_task)), // task itseld can be treated as acceptor
      waiters_semaphore_{} {}

BaseTask::~BaseTask() noexcept(false) {
  if (nreferences_ != 0u) {
    throw api::AliveTaskDeletion("Deleting task with references");
  }
}

// modifiers
void BaseTask::NotifyAboutComplete() noexcept {
  waiters_semaphore_.store(nacceptors_.sub(1u, api::MemoryOrder::relaxed));
  assert(nacceptors_.load(api::MemoryOrder::relaxed) != 255);
  nacceptors_.notify_all();
}

// sync operations
void BaseTask::Wait(const unsigned char expected_value) noexcept(false) {
  auto old{nacceptors_.load(api::MemoryOrder::acquire)};
  // task can be considered as acceptor if is_blocking_task = true, so
  // expacted_value must be corrected with this fact in mind.
  const auto correct_value{expected_value +
                           static_cast<unsigned char>(is_blocking_task_)};
  if (correct_value > old) {
    throw api::Deadlock("Unreachible expected value");
  }

  // Maximum number of waiters = nacceptors (excepcted_value values which may
  // cause thread block: 0, 1..., nacceptors - 1 if is_blocking_task = false OR
  // 1, 2, ..., nacceptors is is_blocking_task = true)
  for (; old != correct_value;
       old = nacceptors_.load(api::MemoryOrder::relaxed)) {
    if (!waiters_semaphore_.sub(1, api::MemoryOrder::relaxed)) {
      throw api::Deadlock("Threads will never be unblocked");
    }
    nacceptors_.wait(old, api::MemoryOrder::relaxed);
  }
}
} // namespace impl

namespace api {
TaskWrapper::TaskWrapper(const PointerType &task,
                         const Allocator &alloc) noexcept(false)
    : task{task}, alloc{alloc} {}

TaskWrapper::~TaskWrapper() noexcept {
  if (task->DecrementNumOfRefs() == 0u) {
    const auto mysize{task->SizeInBytes()};
    std::allocator_traits<Allocator>::destroy(alloc, task);
    std::allocator_traits<Allocator>::deallocate(alloc, task, mysize);
  }
}
} // namespace api
