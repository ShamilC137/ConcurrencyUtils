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
  api::DeferThreadWrapper RegisterThread(api::DeferThread *thread);

  void DeleteThread(const api::ThreadId id) noexcept(false);

  [[nodiscard]] api::ThreadSignals GetThreadSignals(api::ThreadId id) const
      noexcept(false);

  bool SendKillSignal(api::ThreadId id) noexcept;

  bool SendSuspendSignal(api::ThreadId id) noexcept;

  bool SuspendThisThread(const api::ThreadId *const id_hint = nullptr) noexcept;

  bool UnsetSignal(api::ThreadId id, api::ThreadSignal signal) noexcept;

  bool Resume(api::ThreadId id) noexcept;

  // Task manipulation functions
 public:
  // Adds task to task queue. Potentially blocks caller thread if queue is busy.
  void PushToQueue(const api::TaskWrapper &task);

  // internal logic functions
 public:
  // Adds new module to kernel
  void AddModule(impl::AbstractModule *module);

  // Start point for program
  [[nodiscard]] int Run();

  // friends
 private:
  // creates kernel if it does not exist; returns reference to the kernel;
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
