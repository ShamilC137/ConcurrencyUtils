#ifndef APPLICATION_KERNEL_KERNEL_KERNEL_HPP_
#define APPLICATION_KERNEL_KERNEL_KERNEL_HPP_

// Current project
#include "../../API/DataStructures//Multithreading//DeferThread.hpp"
#include "../../API/DataStructures/Containers/HashMap.hpp"
#include "../../API/DataStructures/Containers/String.hpp"
#include "../../API/DataStructures/Containers/Vector.hpp"
#include "../../API/DataStructures/Multithreading/Atomics.hpp"
#include "../../API/DataStructures/Multithreading/Mutex.hpp"
#include "../../API/DataStructures/Multithreading/ScopedLock.hpp"
#include "../../API/DataStructures/Multithreading/Thread.hpp"
#include "../../API/DataStructures/Multithreading/ThreadPool.hpp"
#include "../../API/DataStructures/Multithreading/UnboundedPriorityBlockingQueue.hpp"
#include "../../API/DataStructures/TaskWrapper.hpp"
#include "../../ImplDetails/AbstractModule.hpp"
#include "../../ImplDetails/ImplAPI/KernelAPI.hpp"
#include "../MMU/VirtualMMU.hpp"
#include "../TaskManager.hpp"
#include "../ThreadManager.hpp"

namespace kernel {
// Memory pool size for whole program (including STL)
static constexpr mmu::SizeType kMMUSize{16 * 1024 * 1024};

// max number of iterations in the event loop that could be spent to send tasks
static constexpr unsigned char kMaxTasksPerLoop{10u};

// max number of iterations in the event loop that could be spent to manage
// threads
static constexpr unsigned char kMaxThreadsPerLoop{10u};

enum class KernelStatus : char { kRun = -1, kOk };

/// <summary>
///   Main class. Starts program, calls modules init functions, managing all
///   resources.
///   Functions, marked as 'Parallel', could be executed in parallel mode.
///   Functions, marked as 'Protected', works in protected mode (i.e. if
///   exit_flag is set nothing is happen).
/// </summary>
class Kernel {
  // Ctors
 public:
  Kernel(const Kernel &) = delete;

  Kernel &operator=(const Kernel &) = delete;

  ~Kernel();

 private:
  // throws: std::bad_alloc
  Kernel() noexcept(false);

  // memory management functions
 public:
  /// <summary>
  ///   Allocates memory
  /// </summary>
  /// <param name="nbytes">
  ///   Number of allocated bytes
  /// </param>
  /// <returns>
  ///   Allocated area or nullptr, if exit_flag is set
  /// </returns>
  /// <exception 'std::bad_alloc'>
  ///   Thrown if memory cannot be allocated
  /// </exception>
  /// <protected>
  ///   true
  /// </protected>
  [[nodiscard]] void *Allocate(const std::size_t nbytes) noexcept(false);

  /// <summary>
  ///   deallocates memory by the given pointer with the given size in bytes
  /// </summary>
  /// <param name="ptr">
  ///   Pointer to deletion area
  /// </param>
  /// <param name="nbytes">
  ///   Number of bytes to delete
  /// </param>
  /// <protected>
  ///   true
  /// </protected>
  void Deallocate(void *ptr, const size_t nbytes) noexcept;

  // Thread manipulation functions
 public:
  /// <summary>
  ///   Adds new thread to kernel manipulated threads. If thread with given id
  ///   have already existed nothing happens. If exit_flag_ is set deletes given
  ///   thread immediatly.
  /// </summary>
  /// <param name="thread">
  ///   Thread to add
  /// </param>
  /// <returns>
  ///   Wrapper to added thread or empty wrapper if exit_flag_ is set.
  /// </returns>
  /// <protected>
  ///   true
  /// </protected>
  api::DeferThreadWrapper RegisterThread(api::DeferThread *thread);

  /// <summary>
  ///   Marks thread with given id as deleted and deletes it if thread not
  ///   joinable and all wrappers on this thread are dead
  /// </summary>
  /// <param name="id"></param>
  /// <exception "out_of_range">
  ///   Thrown if thread not belongs to kernel manipulated threads
  /// </exception>
  /// <protected>
  ///   true
  /// </protected>
  void DeleteThread(const api::ThreadId id) noexcept(false);

  /// <summary>
  ///  Returns thread with given id signals.
  /// </summary>
  /// <param name="id">
  ///   Thread id
  /// </param>
  /// <returns>
  ///   Specified thread signals
  /// </returns>
  /// <exception "out_of_range">
  ///   Thrown if thread not belongs to kernel manipulated threads
  /// </exception>
  /// <protected>
  ///   true
  /// </protected>
  [[nodiscard]] api::ThreadSignals GetThreadSignals(api::ThreadId id) const
      noexcept(false);

  /// <summary>
  ///   Send kill signal to specified thread.
  /// </summary>
  /// <param name="id">
  ///   Thread id
  /// </param>
  /// <returns>
  ///   True if signal is sent and false otherwise.
  /// </returns>
  /// <protected>
  ///   true
  /// </protected>
  bool SendKillSignal(api::ThreadId id) noexcept;

  /// <summary>
  ///   Send suspend signal to specified thread.
  /// </summary>
  /// <param name="id">
  ///   Thread id
  /// </param>
  /// <returns>
  ///   True if signal is sent and false otherwise.
  /// </returns>
  /// <protected>
  ///   true
  /// </protected>
  bool SendSuspendSignal(api::ThreadId id) noexcept;

  /// <summary>
  ///   Suspend current thread. Uses thread id hint if one present
  /// </summary>
  /// <param name="id_hint">
  ///   Thread id
  /// </param>
  /// <returns>
  ///   True if signal is sent and false otherwise.
  /// </returns>
  /// <protected>
  ///   true
  /// </protected>
  bool SuspendThisThread(const api::ThreadId *const id_hint = nullptr) noexcept;

  /// <summary>
  ///   Unsets specified signal on specified thread.
  /// </summary>
  /// <param name="id">
  ///   Thread id
  /// </param>
  /// <param name="signal">
  ///   Signal to unset
  /// </param>
  /// <returns>
  ///   True if signal is unsetted and false otherwise
  /// </returns>
  /// <protected>
  ///   true
  /// </protected>
  bool UnsetSignal(api::ThreadId id, api::ThreadSignal signal) noexcept;

  /// <summary>
  ///   Resumes thread with given id.
  /// </summary>
  /// <param name="id">
  ///   Thread id
  /// </param>
  /// <returns>
  ///   True if thread has resumed and false otherwise. If the thread has not
  ///   been suspended, but the call succeeds - true is a valid return
  /// </returns>
  /// <protected>
  ///   true
  /// </protected>
  bool Resume(api::ThreadId id) noexcept;

  // Task manipulation functions
 public:
  /// <summary>
  ///   Adds task to task queue. Potentially blocks caller thread if queue is
  ///   busy. If exit_flag_ is set all tasks are rejected.
  /// </summary>
  /// <param name="task">
  ///   Task to add
  /// </param>
  /// <protected>
  ///   true
  /// </protected>
  void PushToQueue(const api::TaskWrapper &task);

  // internal logic functions
 public:
  /// <summary>
  ///   Adds new module to kernel. Does not take ownnership on module. All
  ///   modules must be added before Run(). If exit_flag_ is set all modules
  ///   are rejected.
  /// </summary>
  /// <param name="module">
  ///   Module to add
  /// </param>
  /// <protected>
  ///   true
  /// </protected>
  void AddModule(impl::AbstractModule *module);

 private:
  // Called on exit
  void OnExitRoutine();

  int EventLoop();

 public:
  /// <summary>
  ///   Program start point. Calls modules' Init() function then starts all
  ///   current DeferThreads.
  /// </summary>
  /// <returns>
  ///   Program error status
  /// </returns>
  [[nodiscard]] int Run();

  /// <summary>
  ///   Program end point. Sets exit flag
  /// </summary>
  void Exit() noexcept;

  // friends
 private:
  /// <summary>
  ///   Creates kernel if not created
  /// </summary>
  /// <returns>
  ///   reference to kernel
  /// </returns>
  friend Kernel &api::kernel_api::GetKernel() noexcept;

  // fields
 private:
  api::AtomicFlag run;
  // memory unit; all allocations pass through it
  mmu::VirtualMMU<kMMUSize> stub;
  api::AtomicFlag exit_flag_;  // shows that program must be closed, i.e. breaks
                               // run loop
  // task manager unit; all task pass through it
  TaskManager task_manager_;
  // thread manager unit; all threads (which created by CreateThread) is pass
  // through it
  ThreadManager thread_manager_;
  api::Vector<ModuleDescriptor> modules_;
};
}  // namespace kernel

#endif  // !APPLICATION_KERNEL_KERNEL_KERNEL_HPP_
