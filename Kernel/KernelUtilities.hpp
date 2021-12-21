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

  // reserved for now
};
}  // namespace kernel
#endif  // APPLICATION_KERNEL_KERNELUTILITIES_HPP_
