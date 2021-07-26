#include "Kernel.hpp"

namespace kernel {
// Ctors
Kernel::Kernel() : mmu_(operator new(kMMUSize)) {} // FIXME: leak
} // namespace kernel

namespace api {
namespace kernel_api {

kernel::Kernel &GetKernel() noexcept {
  static kernel::Kernel kernel{};
  return kernel;
}

[[nodiscard]] mmu::VPtr<void>
Allocate(const std::size_t align, const std::size_t nbytes) noexcept(false) {
  return GetKernel().Allocate(align, nbytes); // throws
}

void Deallocate(mmu::VPtr<void> ptr, const std::size_t nbytes) noexcept {
  GetKernel().Deallocate(ptr, nbytes);
}
} // namespace kernel_api
} // namespace api
