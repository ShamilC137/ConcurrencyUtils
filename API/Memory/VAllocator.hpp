#ifndef APPLICATION_API_MEMORY_VALLOCATOR_HPP_
#define APPLICATION_API_MEMORY_VALLOCATOR_HPP_
// current project
#include "../../API/PublicAPI.hpp"
#include "VPtr.hpp"

// STL
#include <type_traits>

namespace api {
template <class T> class VAllocator {
  static_assert(!std::is_const_v<T>,
                "Container of const types is forbidden");

public:
  using value_type = T;

  using pointer = mmu::VPtr<value_type>;
  using const_pointer = const pointer;

  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using propagate_on_container_move_assignment = std::true_type;
  using propagate_on_container_copy_assignment = std::true_type;
  using propagate_on_container_swap = std::true_type;
  using is_always_equal = std::true_type;

public:
  // ctors
  constexpr VAllocator() noexcept = default;
  constexpr VAllocator(const VAllocator &) noexcept = default;

  template <class Other>
  constexpr VAllocator(const VAllocator<Other> &) noexcept {}

  [[nodiscard]] pointer allocate(const size_type count) {
    decltype(auto) memory{api::Allocate<value_type>(count)};
    return memory;
  }

  void deallocate(pointer ptr, size_type count) {
    api::Deallocate<value_type>(ptr, count);
  }
};
} // namespace api
#endif // !APPLICATION_API_MEMORY_VALLOCATOR_HPP_
