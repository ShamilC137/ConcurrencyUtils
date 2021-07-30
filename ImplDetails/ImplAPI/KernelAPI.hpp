#ifndef APPLICATION_IMPLDETAILS_IMPLAPI_KERNELAPI_HPP_
#define APPLICATION_IMPLDETAILS_IMPLAPI_KERNELAPI_HPP_

// current project
#include "../../API/Memory/VPtr.hpp"

#include "../../Config.hpp"

// STL
#include <cstddef>

// OS dependent headers
#if WINDOWS32 | WINDOWS64
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
[[nodiscard]] mmu::VPtr<void>
Allocate(const std::size_t align, const std::size_t nbytes) noexcept(false);

// deallocates memory by the given pointer with the given size in bytes
void Deallocate(mmu::VPtr<void> ptr, const std::size_t nbytes) noexcept;

// OS dependent functions
#if defined WINDOWS32 | defined WINDOWS64
// Suspends executed thread; takes thread native handler;
// return -1 if failed and non negative number otherwise
::DWORD SuspendThread(::HANDLE thread_native_handler) noexcept;

// Resumes thread execution; takes thread native handler;
// return -1 if failed and non negative number otherwise
::DWORD ResumeThread(::HANDLE thread_native_handler) noexcept;
#endif // !defined WINDOWS32 | defined WINDOWS64

} // namespace kernel_api
} // namespace api

#endif // !APPLICATION_IMPLDETAILS_IMPLAPI_KERNELAPI_HPP_
