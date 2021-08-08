#include "TaskWrapper.hpp"

namespace api {
namespace kernel_api {
void Deallocate(void *ptr, const std::size_t nbytes) noexcept;
}

TaskWrapper::TaskWrapper(const PointerType &task,
                         const api::String &target) noexcept
    : task_{task}, target_{target} {
  task->IncrementNumOfRefs();
}

TaskWrapper::TaskWrapper(const TaskWrapper &rhs) noexcept
    : TaskWrapper(rhs.task_, rhs.target_) {}

TaskWrapper::TaskWrapper(TaskWrapper &&rhs)
    : task_{rhs.task_}, target_{std::move(rhs.target_)} {
  rhs.task_ = nullptr;
}

TaskWrapper &TaskWrapper::operator=(TaskWrapper &&rhs) noexcept {
  task_ = rhs.task_;
  target_ = std::move(rhs.target_);
  rhs.task_ = nullptr;
  return *this;
}

TaskWrapper::~TaskWrapper() noexcept {
  if (task_->DecrementNumOfRefs() == 0u) {
    const auto mysize{task_->SizeInBytes()};
    task_->~BaseTask();
#if ALIGNED_ALLOCATOR_USAGE
    kernel_api::Deallocate(task_, mysize);
#elif STL_ALLOCATOR_USAGE
    ::operator delete(task_, mysize);
#endif
  }
}
} // namespace api
