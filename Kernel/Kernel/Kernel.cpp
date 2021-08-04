#include "Kernel.hpp"

namespace kernel {
// NOTE: function marked as multithreaded can be called from few threads
// and must not block other such functions, so them have its own mutex objects

// Ctors
Kernel::Kernel() noexcept(false) : mmu_(operator new(kMMUSize)) {} // throws

// Multithreading
[[nodiscard]] void *Kernel::Allocate(const std::size_t align,
                                     const std::size_t nbytes) noexcept(false) {
  static api::Mutex mt{};
  api::ScopedLock<api::Mutex> lock(mt);
  return mmu_.Allocate(align, nbytes); // throws
}

// Multithreading
void Kernel::Deallocate(void *ptr, const size_t nbytes) noexcept {
  static api::Mutex mt{};
  api::ScopedLock<api::Mutex> lock(mt);
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
  return GetKernel().Allocate(align, nbytes); // throws
}

void Deallocate(void *ptr, const std::size_t nbytes) noexcept {
  GetKernel().Deallocate(ptr, nbytes);
}
} // namespace kernel_api
} // namespace api
