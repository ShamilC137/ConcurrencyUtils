#ifndef APPLICATION_API_MEMORY_VPTR_HPP_
#define APPLICATION_API_MEMORY_VPTR_HPP_
namespace mmu {
template <class T> class VPtr {
public:
  VPtr(){};
  VPtr(T&) {}
  template <class Other> VPtr(VPtr<Other>) {}

  static VPtr<T> pointer_to(T &) { return {}; }
  T &operator*() { 
    T a{};
    return a; 
  }
};

template <> class VPtr<void> {
public:
  VPtr(){};
  VPtr(void *) {}
 template <class Other>
  VPtr(VPtr<Other>) {}
};
} // namespace mmu

#endif // !APPLICATION_API_MEMORY_VPTR_HPP_
