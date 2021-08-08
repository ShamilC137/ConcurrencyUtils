#ifndef APPLICATION_API_DATASTRUCTURES_TASKWRAPPER_HPP_
#define APPLICATION_API_DATASTRUCTURES_TASKWRAPPER_HPP_

// current project
#include "../../Config.hpp"
#include "Task.hpp"

namespace api {
// Task wrapper that deletes task if number of references on this task becomes 0
// Note: if task type must supply == and < operators. Other operators will be
// implemented by wrapper.
// Wrapper also contains target slot which must be called (mb empty if no slot
// is set). Target is usually set by kernel.
// Note: task itself cannot contain target because one task can be binded to
// few slots (every slot will have its own TaskWrapper)
class TaskWrapper {
public:
  using PointerType = impl::BaseTask *;

  TaskWrapper(const PointerType &task, const api::String &target = {}) noexcept;

  TaskWrapper(const TaskWrapper &rhs) noexcept;

  TaskWrapper(TaskWrapper &&rhs);

  TaskWrapper &operator=(const TaskWrapper &task) = delete;
  TaskWrapper &operator=(TaskWrapper &&task) noexcept;

  [[nodiscard]] inline PointerType GetTask() noexcept { return task_; }

  [[nodiscard]] inline const PointerType GetTask() const noexcept {
    return task_;
  }

  inline void SetTarget(const api::String &target) { target_ = target; }

  [[nodiscard]] inline api::String GetTarget() const { return target_; }

  virtual ~TaskWrapper() noexcept;

protected:
  PointerType task_;

private:
  api::String target_; // Associated with this task slot
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

[[nodiscard]] inline bool operator==(const TaskWrapper &lhs,
                                     const TaskWrapper &rhs) {
  return (*lhs.GetTask()) == (*rhs.GetTask());
}

[[nodiscard]] inline bool operator!=(const TaskWrapper &lhs,
                                     const TaskWrapper &rhs) {
  return !(lhs == rhs);
}

[[nodiscard]] inline bool operator<(const TaskWrapper &lhs,
                                    const TaskWrapper &rhs) {
  return (*lhs.GetTask()) < (*rhs.GetTask());
}

[[nodiscard]] inline bool operator>(const TaskWrapper &lhs,
                                    const TaskWrapper &rhs) {
  return rhs < lhs;
}

[[nodiscard]] inline bool operator>=(const TaskWrapper &lhs,
                                     const TaskWrapper &rhs) {
  return !(lhs < rhs);
}

[[nodiscard]] inline bool operator<=(const TaskWrapper &lhs,
                                     const TaskWrapper &rhs) {
  return !(lhs > rhs);
}
} // namespace api
#endif // !APPLICATION_API_DATASTRUCTURES_TASKWRAPPER_HPP_
