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
static constexpr mmu::SizeType kMMUSize{16 * 1024 * 1024};

//
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
  // allocates memory with the given alignment; takes nbytes to allocate
  // returns pointer to allocated memory
  // throws: std::bad_alloc
  [[nodiscard]] void *Allocate(const std::size_t nbytes) noexcept(false);

  // deallocates memory by the given pointer with the given size in bytes
  void Deallocate(void *ptr, const size_t nbytes) noexcept;

  // Thread manipulation functions
 public:
  /// <summary>
  ///   Adds new thread to kernel manipulated threads. If thread with given id
  ///   have already existed nothing happens.
  /// </summary>
  /// <param name="thread">
  ///   Thread to add
  /// </param>
  /// <returns>
  ///   Wrapper to added thread.
  /// </returns>
  api::DeferThreadWrapper RegisterThread(api::DeferThread *thread);

  /// <summary>
  ///   Marks thread with given id as deleted and deletes it if thread not
  ///   joinable and all wrappers on this thread are dead
  /// </summary>
  /// <param name="id"></param>
  /// <exception "out_of_range">
  ///   Thrown if thread not belongs to kernel manipulated threads
  /// </exception>
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
  bool Resume(api::ThreadId id) noexcept;

  // Task manipulation functions
 public:
  /// <summary>
  ///   Adds task to task queue. Potentially blocks caller thread if queue is
  ///   busy.
  /// </summary>
  /// <param name="task">
  ///   Task to add
  /// </param>
  void PushToQueue(const api::TaskWrapper &task);

  // internal logic functions
 public:
  /// <summary>
  ///   Adds new module to kernel. Does not take ownnership on module. All
  ///   modules must be added before Run().
  /// </summary>
  /// <param name="module">
  ///   Module to add
  /// </param>
  void AddModule(impl::AbstractModule *module);

  /// <summary>
  ///   Program start point. Calls modules' Init() function then starts all
  ///   current DeferThreads.
  /// </summary>
  /// <returns>
  ///   Program error status
  /// </returns>
  [[nodiscard]] int Run();

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
