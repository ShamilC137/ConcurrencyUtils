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

  // Takes pointer to task and probably target slot signature.
  TaskWrapper();
  TaskWrapper(const PointerType &task, const api::String &target = {},
              const api::String &signal = {}) noexcept;

  TaskWrapper(const TaskWrapper &rhs) noexcept;

  TaskWrapper(TaskWrapper &&rhs) noexcept;

  virtual ~TaskWrapper() noexcept;

  TaskWrapper &operator=(const TaskWrapper &task);

  TaskWrapper &operator=(TaskWrapper &&task) noexcept;

  [[nodiscard]] inline PointerType GetTask() noexcept { return task_; }

  [[nodiscard]] inline const PointerType GetTask() const noexcept {
    return task_;
  }

  // Kernel 
  inline void SetTarget(const api::String &target) { target_ = target; }

  [[nodiscard]] inline const api::String &GetTarget() const { return target_; }

  // Kernel
  inline void SetCausedSignal(const api::String &signal) { signal_ = signal; }

  [[nodiscard]] inline const api::String &GetCausedSignal() const noexcept {
    return signal_;
  }

protected:
  PointerType task_;

private:
  api::String target_; // Associated with this task slot
  api::String signal_; // Associated with this task signal
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
