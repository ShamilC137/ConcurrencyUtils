#ifndef APPLICATION_API_DATASTRUCTURES_TASKWRAPPER_HPP_
#define APPLICATION_API_DATASTRUCTURES_TASKWRAPPER_HPP_

// current project
#include "../../Config.hpp"
#include "Task.hpp"

namespace api {
/// <summary>
///   Task wrapper that deletes task if number of references on this task
///   becomes 0.
///   Note: task type must supply == and < operators. Other operators
///   will be implemented by wrapper. Wrapper also contains target slot which
///   must be called(mb empty if no slot is set). Target is usually set by
///   kernel.
///   Note: task itself cannot contain target because one task can be binded to
///   few slots(every slot will have its own TaskWrapper).

/// </summary>
class TaskWrapper {
 public:
  using PointerType = impl::BaseTask *;

  TaskWrapper();

  /// <summary>
  ///   Takes ownership on task
  /// </summary>
  /// <param name="task">
  ///   Pointer to task
  /// </param>
  /// <param name="target">
  ///   Target slot signature (without module id)
  /// </param>
  TaskWrapper(const PointerType &task, const api::String &target) noexcept;

  TaskWrapper(const TaskWrapper &rhs) noexcept;

  TaskWrapper(TaskWrapper &&rhs) noexcept;

  virtual ~TaskWrapper() noexcept;

  TaskWrapper &operator=(const TaskWrapper &task);

  TaskWrapper &operator=(TaskWrapper &&task) noexcept;

  /// <returns>
  ///   Pointer to underlying task
  /// </returns>
  [[nodiscard]] inline PointerType GetTask() noexcept { return task_; }

  /// <returns>
  ///   Pointer to underlying task
  /// </returns>
  [[nodiscard]] inline const PointerType GetTask() const noexcept {
    return task_;
  }

  /// <summary>
  ///   Sets target slot (without module id). Used by kernel.
  /// </summary>
  /// <param name="target">
  ///   Target slot signature
  /// </param>
  inline void SetTarget(const api::String &target) { target_ = target; }

  /// <returns>
  ///   Target slot signature
  /// </returns/
  [[nodiscard]] inline const api::String &GetTarget() const { return target_; }

  /// <returns>
  ///   Caused signal full signature (i.e. with module id)
  /// </returns>
  [[nodiscard]] inline const api::String &GetCausedSignal() const noexcept {
    return task_->GetCausedSignal();
  }

 protected:
  PointerType task_;

 private:
  api::String target_;  // Associated with this task slot
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
}  // namespace api
#endif  // !APPLICATION_API_DATASTRUCTURES_TASKWRAPPER_HPP_
