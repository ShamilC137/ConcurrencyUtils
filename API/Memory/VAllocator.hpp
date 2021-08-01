#ifndef APPLICATION_API_MEMORY_VALLOCATOR_HPP_
#define APPLICATION_API_MEMORY_VALLOCATOR_HPP_
// current project
#include "../../API/PublicAPI.hpp"
#include "VPtr.hpp"

// STL
#include <type_traits>

namespace api {
// Wrapper STL compatible allocator. Contains all methods that allows to work
// with current project. Uses public API methods to implement its methods.
template <class T> class VAllocator {
  static_assert(!std::is_const_v<T>, "Container of const types is forbidden");

public:
  using value_type = T;

  using pointer = VPtr<value_type>;
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

  // Allocates given count of objects
  [[nodiscard]] static pointer allocate(const size_type count) {
    decltype(auto) memory{Allocate<value_type>(count)};
    return memory;
  }

  // Deallocates given counts of objects
  static void deallocate(pointer ptr, size_type count) noexcept {
    Deallocate<value_type>(ptr, count);
  }

  // Constructs object on given pointer with the given arguments.
  template <class... Args>
  static void construct(VPtr<value_type> &ptr, Args &&...args) {
    new (std::addressof(*ptr)) value_type(std::forward<Args>(args)...);
  }

  // Calls destructor of object with the given pointer
  static void destroy(VPtr<value_type> &ptr) noexcept(
      noexcept(std::declval<value_type>().~value_type())) {
    (*ptr).~value_type();
  }

  // allocate + construct
  template <class... Args>
  static VPtr<value_type> AllocateAndConstructOne(Args &&...args) {
    decltype(auto) ptr{allocate(1)};
    construct(ptr, std::forward<Args>(args)...);
    return ptr;
  }

  // destroy + deallocate
  static void DestroyAndDeallocateOne(VPtr<value_type> &what) noexcept(
      noexcept(destroy(what))) {
    destroy(what);
    deallocate(what, sizeof(value_type));
  }
};
} // namespace api
#endif // !APPLICATION_API_MEMORY_VALLOCATOR_HPP_
