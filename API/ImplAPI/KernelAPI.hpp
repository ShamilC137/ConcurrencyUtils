#ifndef APPLICATION_API_IMPLAPI_KERNELAPI_HPP_
#define APPLICATION_API_IMPLAPI_KERNELAPI_HPP_

// current project
#include "../Memory/VPtr.hpp"

// STL
#include <cstddef>

namespace kernel {
class Kernel; // singleton
}

namespace api {
namespace kernel_api {
// Return created kernel object; the only way to create such object
[[nodiscard]] inline kernel::Kernel &GetKernel() noexcept;

// allocates memory with the given alignment; takes nbytes to allocate
// return virtual pointer to allocated memory
// throws
[[nodiscard]] mmu::VPtr<void>
Allocate(const std::size_t align, const std::size_t nbytes) noexcept(false);

// deallocates memory by the given pointer with the given size in bytes
void Deallocate(mmu::VPtr<void> ptr, const std::size_t nbytes) noexcept;

} // namespace kernel_api
} // namespace api

#endif // !APPLICATION_API_IMPLAPI_KERNELAPI_HPP_
