#ifndef APPLICATION_KERNEL_KERNEL_KERNEL_HPP_
#define APPLICATION_KERNEL_KERNEL_KERNEL_HPP_

// Current project
#include "../../API/DataStructures/Containers/HashMap.hpp"
#include "../../API/DataStructures/Containers/String.hpp"
#include "../../API/DataStructures/Containers/Vector.hpp"

#include "../../API/DataStructures/Multithreading/Mutex.hpp"
#include "../../API/DataStructures/Multithreading/ScopedLock.hpp"
#include "../../API/DataStructures/Multithreading/ThreadPool.hpp"
#include "../../API/DataStructures/Multithreading/Thread.hpp"
#include "../../API/DataStructures/Multithreading/UnboundedPriorityBlockingQueue.hpp"
#include "../../ImplDetails/ImplAPI/KernelAPI.hpp"

#include "../MMU/VirtualMMU.hpp"

#include "../AbstractModule.hpp"

// STL
#include <cstddef>

namespace impl {
// The struct contains all necessary to kernel information about associated
// kernel.
struct ModuleDescriptor {
  // Associated module identifier.
  api::String mdl_id;
  // Associated with this module queue of tasks. Module will take task from this
  // queue. Kernel will push tasks to this queue.
  api::UnboundedPriorityBlockingQueue<api::TaskWrapper> mtasks_queue;
  // Associated with this module tasks queue. Kernel will take tasks from this
  // queue. Module will push tasks to this queue.
  api::UnboundedPriorityBlockingQueue<api::TaskWrapper> kntasks_queue;
  // All module must have main threads that will handle problems with other
  // threads (uncatched exception, for example). Such situations will may send
  // signals to main thread or to kernel (which will resend it to main thread).
  // Main thread can relaunch additional thread, kill it, etc. If thread will be
  // canceled without notifying kernel - UB; all additional threads will be
  // canceled otherwise.
  api::Thread mainthr;
  // Shows whether the main thread is suspended or not.
  bool is_mt_suspended;
  // Additional threads. Created in the main thread and control by it. Mb empty
  api::Vector<api::Thread> addl_thrs;
  api::Vector<api::ThreadPool> addl_tpools;
};

struct SlotSigPair {
  api::String slot_sig; // slot signature
  ModuleDescriptor *md; // associated with this slot module descriptor
};
} // namespace impl

namespace kernel {
static constexpr mmu::SizeType kMMUSize{16 * 1024 * 1024};

//
class Kernel {
  // Ctors
private:
  // throws
  Kernel() noexcept(false);

  // memory manage functions
public:
  // allocates memory with the given alignment; takes nbytes to allocate
  // returns pointer to allocated memory
  // throws
  [[nodiscard]] void *Allocate(const std::size_t align,
                               const std::size_t nbytes) noexcept(false);

  // deallocates memory by the given pointer with the given size in bytes
  void Deallocate(void *ptr, const size_t nbytes) noexcept;

  // internal logic functions
public:
  // friends
private:
  // creates kernel if it does not exist; returns reference to the kernel;
  friend Kernel &api::kernel_api::GetKernel() noexcept;

  // fields
private:
  // memory unit; all allocations pass through it
  mmu::VirtualMMU<kMMUSize> mmu_;
  // Containter of connection; connection model:
  // signal -> container of slots (contains pair: slot signature and associated
  // module).
  // Cannot work with objects for now.
  api::HashMap<api::String, api::Vector<impl::SlotSigPair>>
      connections_signatures_;
  // Container of modules.
  // Key: module id, value: associated module descriptor.
  api::HashMap<api::String, impl::ModuleDescriptor> modules_;
};
} // namespace kernel

#endif // !APPLICATION_KERNEL_KERNEL_KERNEL_HPP_
