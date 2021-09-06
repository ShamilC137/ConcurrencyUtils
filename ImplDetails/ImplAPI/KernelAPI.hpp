#ifndef APPLICATION_IMPLDETAILS_IMPLAPI_KERNELAPI_HPP_
#define APPLICATION_IMPLDETAILS_IMPLAPI_KERNELAPI_HPP_

// current project
#include "../../Config.hpp"
#include "../../API/DataStructures/TaskWrapper.hpp"

#include "Errors.hpp"

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
// throws: std::bad_alloc
[[nodiscard]] void *Allocate(const std::size_t align,
                             const std::size_t nbytes) noexcept(false);

// deallocates memory by the given pointer with the given size in bytes
void Deallocate(void *ptr, const std::size_t nbytes) noexcept;

// Adds new task to kernel queue. Takes task (as wrapped one). Potentially
// blocks caller thread if queue is busy.
// Warning: if wrapper object on caller thread is a temporary object,
// it will be deleted after all slots routine complete!
void PushToKernelQueue(const api::TaskWrapper &task);
} // namespace kernel_api
} // namespace api

#endif // !APPLICATION_IMPLDETAILS_IMPLAPI_KERNELAPI_HPP_
