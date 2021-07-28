#ifndef APPLICATION_API_MEMORY_VPTR_HPP_
#define APPLICATION_API_MEMORY_VPTR_HPP_
#include "../../Kernel/MMU/Usings.hpp"
namespace mmu {
template <class T> class VPtr {
public:
  VPtr(){};
  VPtr(SizeType) {}
  VPtr(T &b) : a{&b} {}
  template <class Other> VPtr(VPtr<Other>) {}
  operator SizeType() { return {}; }

  T *operator&() {
    return a;
  }

  static VPtr<T> pointer_to(T &) { return {}; }
  T &operator*() {
    return *a;
  }

  const T &operator*() const {
    return *a;
  }
  T *operator->() {
    return a;
  }
  const T *operator->() const {
    return a;
  }
  
  VPtr<T> &operator=(std::nullptr_t) { return *this; }
  VPtr<T> &operator=(VPtr<T>) { return *this; }

  mmu::VPtr<T> operator+(std::size_t) const {
    return {};
  }

  mmu::VPtr<T> operator-(std::size_t) const {
    return {};
  }

  mmu::VPtr<T> operator-(mmu::VPtr<T>) const {
    return {};
  }

  T *a;
};

template <> class VPtr<void> {
public:
  VPtr(){};
  VPtr(SizeType) {}
  VPtr(void *) {}
  template <class Other> VPtr(VPtr<Other>) {}
  operator SizeType() { return {}; }
  void *operator&() {
    void *a{};
    return a;
  }

  VPtr<void> &operator=(VPtr<void>) { return *this; }
  VPtr &operator=(std::nullptr_t) { return *this; }
};
} // namespace mmu


#endif // !APPLICATION_API_MEMORY_VPTR_HPP_
