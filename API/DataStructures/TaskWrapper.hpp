#ifndef APPLICATION_API_DATASTRUCTURES_TASKWRAPPER_HPP_
#define APPLICATION_API_DATASTRUCTURES_TASKWRAPPER_HPP_

// current project
#include "../../Config.hpp"
#include "Task.hpp"

namespace api {
// Task wrapper that deletes task if number of references on this task becomes 0
class TaskWrapper {
public:
  using PointerType = impl::BaseTask *;

  TaskWrapper(const PointerType &task) noexcept(false);

  TaskWrapper(const TaskWrapper &rhs) noexcept;

  TaskWrapper &operator=(const TaskWrapper &task)  = delete;

  [[nodiscard]] inline PointerType GetTask() noexcept { return task_; }

  [[nodiscard]] inline const PointerType GetTask() const noexcept {
    return task_;
  }

  virtual ~TaskWrapper() noexcept;

protected:
  PointerType task_;
};

template <class ReturnType, class... Args>
struct ReturnTaskWrapper : TaskWrapper {
  using PointerType = ReturnTask<ReturnType, Args...> *;
  
  ReturnTaskWrapper(const PointerType &task) : TaskWrapper(task) {}

  [[nodiscard]] inline PointerType GetTask() noexcept {
    return static_cast<PointerType>(task_);
  }

  [[nodiscard]] inline const PointerType GetTask() const noexcept {
    return static_cast<const PointerType>(task_);
  }
};
} // namespace api
#endif // !APPLICATION_API_DATASTRUCTURES_TASKWRAPPER_HPP_
