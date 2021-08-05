#include "TaskWrapper.hpp"

namespace api {
namespace kernel_api {
void Deallocate(void *ptr, const std::size_t nbytes) noexcept;
}

TaskWrapper::TaskWrapper(const PointerType &task) noexcept(false)
    : task_{task} {
  task->IncremenetNumOfRefs();
}

TaskWrapper::TaskWrapper(const TaskWrapper &rhs) noexcept
    : TaskWrapper(rhs.task_) {}

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
