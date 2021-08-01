#include "Kernel.hpp"

namespace kernel {
// Ctors
Kernel::Kernel() noexcept(false)
    : mmu_(operator new(kMMUSize)) {} // throws
} // namespace kernel

namespace api {
namespace kernel_api {

kernel::Kernel &GetKernel() noexcept {
  static kernel::Kernel kernel{};
  return kernel;
}

[[nodiscard]] api::VPtr<void>
Allocate(const std::size_t align, const std::size_t nbytes) noexcept(false) {
  return GetKernel().Allocate(align, nbytes); // throws
}

void Deallocate(api::VPtr<void> ptr, const std::size_t nbytes) noexcept {
  GetKernel().Deallocate(ptr, nbytes);
}

::DWORD SuspendThread(::HANDLE thread_native_handler) noexcept {
  return ::SuspendThread(thread_native_handler);
}

::DWORD ResumeThread(::HANDLE thread_native_handler) noexcept {
  return ::ResumeThread(thread_native_handler);
}
} // namespace kernel_api
} // namespace api
