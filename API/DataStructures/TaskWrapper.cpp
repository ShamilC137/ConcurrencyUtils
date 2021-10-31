#include "TaskWrapper.hpp"

namespace api {
TaskWrapper::TaskWrapper() : task_{}, target_{} {}
TaskWrapper::TaskWrapper(const PointerType &task, const api::String &target,
                         const api::String &signal) noexcept
    : task_{task}, target_{target}, signal_{signal} {
  if (task_) {
    task->IncrementNumOfRefs(api::MemoryOrder::release);
  }
}

TaskWrapper::TaskWrapper(const TaskWrapper &rhs) noexcept
    : TaskWrapper(rhs.task_, rhs.target_) {}

TaskWrapper::TaskWrapper(TaskWrapper &&rhs) noexcept
    : task_{rhs.task_}, target_{std::move(rhs.target_)} {
  rhs.task_ = nullptr;
}

TaskWrapper &TaskWrapper::operator=(const TaskWrapper &rhs) {
  this->~TaskWrapper();        // clear current context
  new (this) TaskWrapper(rhs); // copy constructor call
  return *this;
}

TaskWrapper &TaskWrapper::operator=(TaskWrapper &&rhs) noexcept {
  this->~TaskWrapper();                   // clear current context
  new (this) TaskWrapper(std::move(rhs)); // move constructor call
  return *this;
}

TaskWrapper::~TaskWrapper() noexcept {
  if (task_ && task_->DecrementNumOfRefs(api::MemoryOrder::release) == 0u) {
    const auto mysize{task_->SizeInBytes()};
    // ensure that task without slots arguments are deleted
    task_->ClearArguments();
    task_->~BaseTask();
    ::operator delete(task_, mysize);
  }
}
} // namespace api
