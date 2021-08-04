#ifndef APPLICATION_IMPLDETAILS_IMPLAPI_KERNELAPI_HPP_
#define APPLICATION_IMPLDETAILS_IMPLAPI_KERNELAPI_HPP_

// current project
#include "../../Config.hpp"

// STL
#include <cstddef>

// OS dependent headers
#if WINDOWS32 | WINDOWS64
#define WIN32_LEAN_AND_MEAN // What is it:
                            // https://devblogs.microsoft.com/oldnewthing/20091130-00/?p=15863
                            // Must be used to avoid problem with socket_types.h
#include <Windows.h>
#else
static_assert(false, "Unsupported OS");
#endif // !WINODOWS32 | WINDOWS64

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
[[nodiscard]] void *Allocate(const std::size_t align,
                             const std::size_t nbytes) noexcept(false);

// deallocates memory by the given pointer with the given size in bytes
void Deallocate(void *ptr, const std::size_t nbytes) noexcept;
} // namespace kernel_api
} // namespace api

#endif // !APPLICATION_IMPLDETAILS_IMPLAPI_KERNELAPI_HPP_
