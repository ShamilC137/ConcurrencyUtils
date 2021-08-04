#ifndef APPLICATION_API_PUBLICAPI_HPP_
#define APPLICATION_API_PUBLICAPI_HPP_

// STL
#include <cstddef>

namespace api {
// iternal functions
namespace kernel_api {
extern [[nodiscard]] void *Allocate(const std::size_t align,
                                    const std::size_t nbytes) noexcept(false);

extern void Deallocate(void *ptr, const std::size_t nbytes) noexcept;
} // namespace kernel_api

// Allocates continious memory block for the given number of objects of T type
// throws
template <class T>
[[nodiscard]] T *Allocate(std::size_t count) noexcept(false) {
  return static_cast<T *>(
      kernel_api::Allocate(alignof(T), sizeof(T) * count)); // throws
}

// Deallocates continious memory block for the given number of objects of type T
// with the given pointer
template <class T> void Deallocate(T *ptr, std::size_t count) noexcept {
  kernel_api::Deallocate(ptr, sizeof(T) * count);
}
} // namespace api

#endif // !APPLICATION_API_PUBLICAPI_HPP_
