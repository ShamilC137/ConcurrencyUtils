#ifndef APPLICATION_KERNEL_KERNELUTILITIES_HPP_
#define APPLICATION_KERNEL_KERNELUTILITIES_HPP_
#include "../API/DataStructures/Multithreading/DeferThread.hpp"
#include "../API/DataStructures/Multithreading/ThreadPool.hpp"
#include "../ImplDetails/AbstractModule.hpp"

namespace kernel {
// The struct contains all necessary to kernel information about associated
// module.
struct ModuleDescriptor {
  // Pointer to associated module.
  impl::AbstractModule *module;
  // All module must have main threads that will handle problems with other
  // threads (uncatched exception, for example). Such situations will may send
  // signals to main thread or to kernel (which will resend it to main thread).
  // Main thread can relaunch additional thread, kill it, etc. If thread will be
  // canceled without notifying kernel - UB; all additional threads will be
  // canceled otherwise.
  api::DeferThread *mainthr;
  // Shows whether the main thread is suspended or not.
  bool is_mt_suspended;
  // Additional threads. Created in the main thread and control by it. Mb empty
  api::Vector<api::DeferThread> addl_thrs;
  api::Vector<api::ThreadPool> addl_tpools;
};
}  // namespace kernel
#endif  // APPLICATION_KERNEL_KERNELUTILITIES_HPP_
