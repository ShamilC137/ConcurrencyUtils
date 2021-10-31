#include "Kernel.hpp"
namespace kernel {
// NOTE: function marked as "Parallel" can be called from few threads
// and must not block other such functions, but block other call of themselfs

// Ctors
Kernel::Kernel() noexcept(false) : mmu_(kMMUSize) /*throws std::bad_alloc*/ {}

// FIXME: Multithreading
[[nodiscard]] void *Kernel::Allocate(const std::size_t nbytes) noexcept(false) {
  return mmu_.Allocate(nbytes); // throws: std::bad_alloc
}

// FIXME: Multithreading
void Kernel::Deallocate(void *ptr, const size_t nbytes) noexcept {
  mmu_.Deallocate(ptr, nbytes);
}

// FIXME: stub
void Kernel::AddModule(impl::AbstractModule * /*module*/) {}

// FIXME: stub
[[nodiscard]] int Kernel::Run() { return {}; }
} // namespace kernel

namespace api {
namespace kernel_api {
kernel::Kernel &GetKernel() noexcept {
  static kernel::Kernel kernel{};
  return kernel;
}

[[nodiscard]] void *Allocate(const std::size_t nbytes) noexcept(false) {
  return GetKernel().Allocate(nbytes); // throws: std::bad_alloc
}

void Deallocate(void *ptr, const std::size_t nbytes) noexcept {
  GetKernel().Deallocate(ptr, nbytes);
}

void PushToKernelQueue(const api::TaskWrapper &task) {
  GetKernel().PushToQueue(task);
}

void AddModule(impl::AbstractModule *module) { GetKernel().AddModule(module); }

[[nodiscard]] int Run() { return GetKernel().Run(); }

// FIXME: stub
[[nodiscard]] const volatile api::ThreadSignals &
GetThreadSignalsReference(const std::size_t hashed_id) noexcept {
  const volatile api::ThreadSignals sigs{};
  return sigs;
}

// FIXME: stub
bool SendKillThreadSignal(const std::size_t hashed_id) noexcept {
  return false;
}

// FIXME: stub
bool SendSuspendThreadSignal(const std::size_t hashed_id) noexcept {
  return false;
}

// FIXME: stub
bool ResumeThread(const std::size_t hashed_id) noexcept { return false; }

// FIXME: stub
void UnsetSignal(const std::size_t hashed_id, api::ThreadSignal sig) noexcept {
  
}

// FIXME: stub
void DeleteThread(const std::size_t hashed_id) {

}

} // namespace kernel_api
} // namespace api

// global scope
// global operator new and global operator delete
// FIXME::SAL warnings

[[nodiscard]] void *operator new(std::size_t count) noexcept(false) {
#if ALIGNED_ALLOCATOR_USAGE
  return api::kernel_api::Allocate(count);
#elif STL_ALLOCATOR_USAGE
  // since we overload operator new, we throw exception by outselves
  auto ptr{operator new (count, std::nothrow_t{})};
  if (ptr) {
    return ptr;
  } else {
    throw std::bad_alloc{};
  }
#endif
}

void operator delete(void *ptr, std::size_t count) noexcept {
#if ALIGNED_ALLOCATOR_USAGE
  api::kernel_api::Deallocate(ptr, count);
#elif STL_ALLOCATOR_USAGE
  operator delete(ptr);
#endif
}
// !global scope
