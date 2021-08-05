#ifndef APPLICATION_API_MEMORY_ALIGNEDALLOCATOR_HPP_
#define APPLICATION_API_MEMORY_ALIGNEDALLOCATOR_HPP_

// current project
#include "../MemoryManagementUtilities.hpp"

// STL
#include <type_traits>

namespace api {
// Wrapper STL compatible allocator. Contains all methods that allows to work
// with current project. Uses public API methods to implement its methods.
template <class T> class AlignedAllocator {
  static_assert(!std::is_const_v<T>, "Container of const types is forbidden");

public:
  using value_type = T;

  using pointer = value_type *;
  using const_pointer = const pointer;

  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using propagate_on_container_move_assignment = std::true_type;
  using propagate_on_container_copy_assignment = std::true_type;
  using propagate_on_container_swap = std::true_type;
  using is_always_equal = std::true_type;

public:
  // ctors
  constexpr AlignedAllocator() noexcept = default;
  constexpr AlignedAllocator(const AlignedAllocator &) noexcept = default;

  template <class Other>
  constexpr AlignedAllocator(const AlignedAllocator<Other> &) noexcept {}

  // Allocates given count of objects
  [[nodiscard]] static pointer allocate(const size_type count) {
    return Allocate<value_type>(count);
  }

  // Deallocates given counts of objects
  static void deallocate(pointer ptr, size_type count) noexcept {
    Deallocate<value_type>(ptr, count);
  }
};
} // namespace api
#endif // !APPLICATION_API_MEMORY_ALIGNEDALLOCATOR_HPP_
