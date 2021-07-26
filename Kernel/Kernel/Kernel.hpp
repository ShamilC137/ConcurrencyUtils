#ifndef APPLICATION_KERNEL_KERNEL_KERNEL_HPP_
#define APPLICATION_KERNEL_KERNEL_KERNEL_HPP_

// Current project
#include "../../API/Containers/HashMap.hpp"
#include "../../API/Containers/String.hpp"
#include "../../API/Containers/Vector.hpp"
#include "../../API/ImplAPI/KernelAPI.hpp"
#include "../MMU/VirtualMMU.hpp"

namespace kernel {

static constexpr mmu::SizeType kMMUSize{16 * 1024 * 1024};

//
class Kernel {
  // Ctors
private:
  Kernel();

  // memory manage functions
public:
  // allocates memory with the given alignment; takes nbytes to allocate
  // return virtual pointer to allocated memory
  // throws
  [[nodiscard]] inline mmu::VPtr<void>
  Allocate(const std::size_t align, const std::size_t nbytes) noexcept(false) {
    return mmu_.Allocate(align, nbytes); // throws
  }

  // deallocates memory by the given pointer with the given size in bytes
  inline void Deallocate(mmu::VPtr<void> &ptr, const size_t nbytes) {
    mmu_.Deallocate(ptr, nbytes);
  }

private:
  // friends
  // returns the kernel
  friend Kernel &api::kernel_api::GetKernel() noexcept;

  // fields
private:
  // memory unit; all allocations pass through it
  mmu::VirtualMMU<kMMUSize> mmu_; // containter of connection; connection model:
                                  // signal -> container of slots
  // contains signatures of connected functions
  // cannot work with objects now
  api::HashMap<api::String, api::Vector<api::String>> connections_signatures_;
};
} // namespace kernel

#endif // !APPLICATION_KERNEL_KERNEL_KERNEL_HPP_
