#ifndef APPLICATION_API_MEMORY_VPTR_HPP_
#define APPLICATION_API_MEMORY_VPTR_HPP_
#include "../../Kernel/MMU/Usings.hpp"
namespace api {
template <class T> class VPtr {
public:
  VPtr(){};
  VPtr(mmu::SizeType) {}
  VPtr(T &b) : a{std::addressof(b)} {}
  VPtr(T *b) : a{b} {}
  VPtr(VPtr<void> &) {}
  VPtr(std::nullptr_t) {}
  T &operator[](std::size_t) { return *a; }
  const T &operator[](std::size_t) const { return *a; }
  operator mmu::SizeType() { return {}; }
  operator T *() { return nullptr; }

  T *operator&() { return a; }

  static VPtr<T> pointer_to(T &) { return {}; }
  T &operator*() { return *a; }

  const T &operator*() const { return *a; }
  T *operator->() { return a; }
  const T *operator->() const { return a; }

  VPtr<T> &operator=(std::nullptr_t) { return *this; }
  VPtr<T> &operator=(VPtr<T>) { return *this; }

  VPtr<T> operator+(std::size_t) const { return {}; }

  VPtr<T> operator-(std::size_t) const { return {}; }

  VPtr<T> operator-(VPtr<T>) const { return {}; }

  T *a;
};

template <> class VPtr<void> {
public:
  VPtr(){};
  VPtr(mmu::SizeType) {}
  VPtr(void *) {}
  template <class Other> VPtr(VPtr<Other>) {}
  operator mmu::SizeType() { return {}; }
  void *operator&() {
    void *a{};
    return a;
  }

  VPtr<void> &operator=(VPtr<void>) { return *this; }
  VPtr &operator=(std::nullptr_t) { return *this; }
};
} // namespace api

#endif // !APPLICATION_API_MEMORY_VPTR_HPP_
