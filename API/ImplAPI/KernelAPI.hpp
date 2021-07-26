#ifndef APPLICATION_API_IMPLAPI_KERNELAPI_HPP_
#define APPLICATION_API_IMPLAPI_KERNELAPI_HPP_
#include "../../Kernel/MMU/Usings.hpp"
#include "../Memory/VPtr.hpp"

namespace kernel {
class Kernel; // singleton 
}

namespace api {
namespace kernel_api {
// Return created kernel object; the only way to create such object
[[nodiscard]] inline kernel::Kernel &GetKernel() noexcept;

// Allocates continious memory block for the given number of objects of T type
// throws
template <class T>
[[nodiscard]] mmu::VPtr<T> Allocate(mmu::SizeType count) noexcept(false) {
  return static_cast<mmu::VPtr<T>>(
      GetKernel().Allocate(alignof(T), sizeof(T) * count)); // throws
}

// Deallocates continious memory block for the given number of objects of type T
// with the given pointer
template <class T>
void Deallocate(mmu::VPtr<T> &ptr, mmu::SizeType count) noexcept {
  GetKernel().Deallocate(ptr, sizeof(T) * count);
}

} // namespace kernel_api
} // namespace api

#endif // !APPLICATION_API_IMPLAPI_KERNELAPI_HPP_
