#ifndef APPLICATION_KERNEL_THREADMANAGER_HPP_
#define APPLICATION_KERNEL_THREADMANAGER_HPP_

// current project
#include "../API/DataStructures/Containers/Deque.hpp"
#include "../API/DataStructures/Containers/HashMap.hpp"
#include "../API/DataStructures/Multithreading/DeferThread.hpp"
#include "../API/DataStructures/Multithreading/DeferThreadWrapper.hpp"
#include "../API/DataStructures/Multithreading/ThreadPool.hpp"
#include "../ImplDetails/Utility.hpp"

namespace kernel {
class ThreadManager {
 public:
  ~ThreadManager() noexcept;

 public:
  template <class ExceptionHandler, class ThreadRoutine, class... RoutineArgs>
  api::DeferThreadWrapper CreateThread(const bool exit_after_call_flag,
                                       ExceptionHandler &&handler,
                                       ThreadRoutine &&routine,
                                       RoutineArgs &&...args);

  void DeleteThread(const api::ThreadId id) noexcept(false);

  api::OperationResult ManageClosedThread();

 public:
  [[nodiscard]] const api::ThreadSignals volatile &GetThreadSignalsReference(
      const api::ThreadId id) noexcept(false);

  bool SetKillSignal(const api::ThreadId id) noexcept;

  bool SetSuspendSignal(const api::ThreadId id) noexcept;

  bool ResumeThread(const api::ThreadId id) noexcept;

  void SuspendThisThread(const api::ThreadId *id_hint) noexcept(false);

  bool UnsetSignal(const api::ThreadId id, api::ThreadSignal signal) noexcept;

 private:
  api::HashMap<api::ThreadId, api::DeferThread *> threads_;
  api::Deque<api::DeferThread *> closed_threads_;
};

template <class ExceptionHandler, class ThreadRoutine, class... RoutineArgs>
inline api::DeferThreadWrapper ThreadManager::CreateThread(
    const bool exit_after_call_flag, ExceptionHandler &&handler,
    ThreadRoutine &&routine, RoutineArgs &&...args) {
  auto thread{new api::DeferThread(exit_after_call_flag,
                                   std::forward<ExceptionHandler>(handler),
                                   std::forward<ThreadRoutine>(routine),
                                   std::forward<RoutineArgs>(args)...)};
  threads_[thread->GetId()] = thread;
  return api::DeferThreadWrapper(thread);
}

}  // namespace kernel
#endif  // APPLICATION_KERNEL_THREADMANAGER_HPP_
