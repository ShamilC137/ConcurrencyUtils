#ifndef APPLICATION_API_MEMORY_VPTR_HPP_
#define APPLICATION_API_MEMORY_VPTR_HPP_
namespace mmu {
template <class T> class VPtr {
public:
  VPtr(){};
  template <class Other> VPtr(VPtr<Other>) {}
};
} // namespace mmu

#endif // !APPLICATION_API_MEMORY_VPTR_HPP_
