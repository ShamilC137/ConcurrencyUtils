#include "Kernel.hpp"

namespace kernel {
// Ctors
Kernel::Kernel() : mmu_(operator new(kMMUSize)) {}
} // namespace kernel

namespace api {
namespace kernel_api {

kernel::Kernel &GetKernel() noexcept {
  static kernel::Kernel kernel{};
  return kernel;
}
} // namespace kernel_api
} // namespace api
