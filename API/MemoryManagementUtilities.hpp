#ifndef APPLICATION_API_MEMORYMANAGEMENTUTILITIES_HPP_
#define APPLICATION_API_MEMORYMANAGEMENTUTILITIES_HPP_

// STL
#include <cstddef>

namespace api {
namespace kernel_api {
// takes nbytes to allocate
// return virtual pointer to allocated memory
// throws: std::bad_alloc
[[nodiscard]] void *Allocate(const std::size_t nbytes) noexcept(false);

// deallocates memory by the given pointer with the given size in bytes
void Deallocate(void *ptr, const std::size_t nbytes) noexcept;
} // namespace kernel_api
} // namespace api

#endif // !APPLICATION_API_MEMORYMANAGEMENTUTILITIES_HPP_
