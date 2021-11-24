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
  /// <summary>
  /// Creates new thread, adds it to threads' table
  /// </summary>
  /// <typeparam name="ExceptionHandler"> exception handler type </typeparam>
  /// <typeparam name="ThreadRoutine">
  ///   underlying thread function type
  /// </typeparam>
  /// <typeparam name="...RoutineArgs">
  ///   thread function arguments
  /// </typeparam>
  /// <param name="exit_after_call_flag">
  ///   is should exit after given function call flag
  /// </param>
  /// <param name="handler">
  ///   exception handler (if exception inherited from std::exception is raised)
  /// </param>
  /// <param name="routine"> underlying thread routine </param>
  /// <param name="...args"> thread routine arguments</param>
  /// <returns> wrapper to created thread </returns>
  /// <multithreading> unsafe </multithreading>
  template <class ExceptionHandler, class ThreadRoutine, class... RoutineArgs>
  inline api::DeferThreadWrapper CreateThread(const bool exit_after_call_flag,
                                              ExceptionHandler &&handler,
                                              ThreadRoutine &&routine,
                                              RoutineArgs &&...args);

  /// <summary>
  ///   Deletes the thread from the threads' table. Closes the thread and, if
  ///   the thread is available for deletion, deletes it (i.e. from memory)
  ///   otherwise adds this thread to closed threads container.
  /// </summary>
  /// <param name="id"> thread id </param>
  /// <returns></returns>
  /// <exception type="std:out_of_range">
  ///    Thrown if the thread with given id is not found
  /// </exception>
  /// <multithreading> unsafe </multithreading>
  void DeleteThread(const api::ThreadId id) noexcept(false);

  /// <summary>
  ///   Checks the closed threads container. If one is available for deletion -
  ///   deletes it.
  /// </summary>
  /// <returns>
  ///   kNone if closed thread container is empty
  ///   kFail if all thread are busy
  ///   kSucces if thread is deleted
  /// </ returns>
  /// <multithreading> unsafe </multithreading>
  api::OperationResult ManageClosedThread();

 public:
  /// <summary>
  ///   Returns the associated with thread with given id signals
  /// </summary>
  /// <param name="id"> thread id </param>
  /// <returns> thread signals </returns>
  /// <exception type="std::out_of_range">
  ///   Thrown if a thread with given id not exists
  /// </exception>
  /// <multithreading> unsafe </multithreading>
  [[nodiscard]] const api::ThreadSignals volatile &GetThreadSignalsReference(
      const api::ThreadId id) noexcept(false);

  /// <summary>
  ///   Sets kill signal on associated with given id thread.
  ///   If thread with given id not exists,returns false.
  /// </summary>
  /// <param name="id"> thread id </param>
  /// <returns> true if signal is setted, otherwise false </returns>
  /// <multithreading> unsafe </multithreading>
  bool SetKillSignal(const api::ThreadId id) noexcept;

  /// <summary>
  ///   Sets suspend signal on associated with given id thread.
  ///   If thread with given id not exists, returns false.
  /// </summary>
  /// <param name="id"> thread id </param>
  /// <returns>true if signal is setted, otherwise false</returns>
  /// <multithreading> unsafe </multithreading>
  bool SetSuspendSignal(const api::ThreadId id) noexcept;

  /// <summary>
  ///   Resumes associated with given id thread, i.e. calls Activate.
  ///   If thread with given id not exists, returns false.
  /// </summary>
  /// <param name="id">thread id</param>
  /// <returns>true if thread is resumed, otherwise false</returns>
  /// <multithreading> unsafe </multithreading>
  bool ResumeThread(const api::ThreadId id) noexcept;

  /// <summary>
  ///   Suspends callee thread by calling DeactivateThread. Uses the id_hint if
  ///   present.
  /// </summary>
  /// <param name="id_hint"> thread id or nullptr </param>
  /// <exception type="std::out_of_range">
  ///   Thrown if given id not belongs to threads' table
  /// </exception>
  /// <multithreading> unsafe </multithreading>
  void SuspendThisThread(const api::ThreadId *id_hint) noexcept(false);

  /// <summary>
  ///   Unsets given signal on thread with given id. If thread not belongs to
  ///   threads' table, returns false.
  /// </summary>
  /// <param name="id"> thread id </param>
  /// <param name="signal"> thread signal </param>
  /// <returns> true if signal is unsetted and false otherwise </returns>
  /// <multithreading> unsafe </multithreading>
  bool UnsetSignal(const api::ThreadId id, api::ThreadSignal signal) noexcept;

 private:
  /// <summary>
  ///   table of all threads, that created with CreateThread.
  ///   Key - thread id, value - pointer to associated thread. Own threads.
  /// </summary>
  api::HashMap<api::ThreadId, api::DeferThread *> threads_;
  /// <summary>
  ///   Container of closed threads. Owns threads.
  /// </summary>
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
