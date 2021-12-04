#ifndef APPLICATION_KERNEL_THREADMANAGER_HPP_
#define APPLICATION_KERNEL_THREADMANAGER_HPP_

// current project
#include "../API/DataStructures/Containers/Deque.hpp"
#include "../API/DataStructures/Containers/HashMap.hpp"
#include "../API/DataStructures/Multithreading/DeferThread.hpp"
#include "../API/DataStructures/Multithreading/DeferThreadWrapper.hpp"
#include "../API/DataStructures/Multithreading/Mutex.hpp"
#include "../API/DataStructures/Multithreading/ScopedLock.hpp"
#include "../API/DataStructures/Multithreading/SharedLockGuard.hpp"
#include "../API/DataStructures/Multithreading/SharedMutex.hpp"
#include "../ImplDetails/Utility.hpp"

namespace kernel {
class ThreadManager {
 public:
  ~ThreadManager() noexcept;

 public:
  /// <summary>
  ///   Adds given thread to threads' table. Takes ownership on thread.
  /// </summary>
  /// <param name="thread">
  ///   Allocated thread
  /// </param>
  /// <returns>
  ///   Wrapper to given thread
  /// </returns>
  api::DeferThreadWrapper AddThread(api::DeferThread *thread);

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
  /// <multithreading> safe </multithreading>
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
  /// <multithreading> safe </multithreading>
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
  /// <multithreading> safe </multithreading>
  [[nodiscard]] api::ThreadSignals GetThreadSignals(
      const api::ThreadId id) const noexcept(false);

  /// <summary>
  ///   Sets kill signal on associated with given id thread.
  ///   If thread with given id not exists,returns false.
  /// </summary>
  /// <param name="id"> thread id </param>
  /// <returns> true if signal is setted, otherwise false </returns>
  /// <multithreading> safe </multithreading>
  bool SendKillSignal(const api::ThreadId id) noexcept;

  /// <summary>
  ///   Sets suspend signal on associated with given id thread.
  ///   If thread with given id not exists, returns false.
  /// </summary>
  /// <param name="id"> thread id </param>
  /// <returns>true if signal is setted, otherwise false</returns>
  /// <multithreading> safe </multithreading>
  bool SetSuspendSignal(const api::ThreadId id) noexcept;

  /// <summary>
  ///   Resumes associated with given id thread, i.e. calls Activate.
  ///   If thread with given id not exists, returns false.
  /// </summary>
  /// <param name="id">thread id</param>
  /// <returns>true if thread is resumed, otherwise false</returns>
  /// <multithreading> safe </multithreading>
  bool ResumeThread(const api::ThreadId id) noexcept;

  /// <summary>
  ///   Suspends callee thread by calling DeactivateThread. Uses the id_hint if
  ///   present.
  /// </summary>
  /// <param name="id_hint"> thread id or nullptr </param>
  /// <exception type="std::out_of_range">
  ///   Thrown if given id not belongs to threads' table
  /// </exception>
  /// <multithreading> safe </multithreading>
  void SuspendThisThread(const api::ThreadId *id_hint) noexcept(false);

  /// <summary>
  ///   Unsets given signal on thread with given id. If thread not belongs to
  ///   threads' table, returns false.
  /// </summary>
  /// <param name="id"> thread id </param>
  /// <param name="signal"> thread signal </param>
  /// <returns> true if signal is unsetted and false otherwise </returns>
  /// <multithreading> safe </multithreading>
  bool UnsetSignal(const api::ThreadId id, api::ThreadSignal signal) noexcept;

 private:
  /// <summary>
  ///   table of all threads, that created with CreateThread.
  ///   Key - thread id, value - pointer to associated thread. Own threads.
  ///   Guarded by shared_mutex.
  /// </summary>
  api::HashMap<api::ThreadId, api::DeferThread *> threads_;
  /// <summary>
  ///   Container of closed threads. Owns threads.
  ///   Guarded by mutex.
  /// </summary>
  api::Deque<api::DeferThread *> closed_threads_;

  mutable api::SharedMutex threads_mutex_;
  mutable api::Mutex closed_threads_mutex_;
};
}  // namespace kernel
#endif  // APPLICATION_KERNEL_THREADMANAGER_HPP_
