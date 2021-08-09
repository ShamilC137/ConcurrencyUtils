#include "Kernel.hpp"

namespace kernel {
// NOTE: function marked as "Parallel" can be called from few threads
// and must not block other such functions, but block other call of themselfs

// Ctors
Kernel::Kernel() noexcept(false) : mmu_(operator new(kMMUSize)) {} // throws

// FIXME: Multithreading
[[nodiscard]] void *Kernel::Allocate(const std::size_t align,
                                     const std::size_t nbytes) noexcept(false) {
  return mmu_.Allocate(align, nbytes); // throws: std::bad_alloc
}

// FIXME: Multithreading
void Kernel::Deallocate(void *ptr, const size_t nbytes) noexcept {
  mmu_.Deallocate(ptr, nbytes);
}
} // namespace kernel

namespace api {
namespace kernel_api {

kernel::Kernel &GetKernel() noexcept {
  static kernel::Kernel kernel{};
  return kernel;
}

[[nodiscard]] void *Allocate(const std::size_t align,
                             const std::size_t nbytes) noexcept(false) {
  return GetKernel().Allocate(align, nbytes); // throws: std::bad_alloc
}

void Deallocate(void *ptr, const std::size_t nbytes) noexcept {
  GetKernel().Deallocate(ptr, nbytes);
}

impl::KernelQueueErrorStatus PushToKernelQueue(const api::TaskWrapper &task) {
  return {};
}
} // namespace kernel_api
} // namespace api
