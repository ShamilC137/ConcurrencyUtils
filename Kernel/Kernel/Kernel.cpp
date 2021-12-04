#include "Kernel.hpp"
#ifdef STL_ALLOCATOR_USAGE
#include <new>
#endif

namespace kernel {
// NOTE: function marked as "Parallel" can be called from few threads
// and must not block other such functions, but block other call of themselfs

Kernel::~Kernel() {}

// Ctors
Kernel::Kernel() noexcept(false) : stub(kMMUSize) /*throws std::bad_alloc*/ {}

// FIXME: Multithreading
[[nodiscard]] void *Kernel::Allocate(const std::size_t nbytes) noexcept(false) {
  return stub.Allocate(nbytes);  // throws: std::bad_alloc
}

// FIXME: Multithreading
void Kernel::Deallocate(void *ptr, const size_t nbytes) noexcept {
  stub.Deallocate(ptr, nbytes);
}

void Kernel::DeleteThread(const api::ThreadId id) noexcept(false) {
  thread_manager_.DeleteThread(id);
}

api::ThreadSignals Kernel::GetThreadSignals(api::ThreadId id) const
    noexcept(false) {
  return thread_manager_.GetThreadSignals(id);
}

bool Kernel::SendKillSignal(api::ThreadId id) noexcept {
  return thread_manager_.SendKillSignal(id);
}

bool Kernel::SendSuspendSignal(api::ThreadId id) noexcept {
  return thread_manager_.SetSuspendSignal(id);
}

bool Kernel::SuspendThisThread(const api::ThreadId *const id_hint) noexcept {
  try {
    thread_manager_.SuspendThisThread(id_hint);
  } catch (...) {
    return false;
  }
  return true;
}

bool Kernel::UnsetSignal(api::ThreadId id, api::ThreadSignal signal) noexcept {
  return thread_manager_.UnsetSignal(id, signal);
}

bool Kernel::Resume(api::ThreadId id) noexcept {
  return thread_manager_.ResumeThread(id);
}

void Kernel::PushToQueue(const api::TaskWrapper &task) {
  task_manager_.PushTask(task);
}

// FIXME: stub
void Kernel::AddModule(impl::AbstractModule * /*module*/) {}

// FIXME: stub
[[nodiscard]] int Kernel::Run() { return {}; }
}  // namespace kernel

namespace api {
namespace kernel_api {
kernel::Kernel &GetKernel() noexcept {
  static kernel::Kernel kernel{};
  return kernel;
}

[[nodiscard]] void *Allocate(const std::size_t nbytes) noexcept(false) {
  return GetKernel().Allocate(nbytes);  // throws: std::bad_alloc
}

void Deallocate(void *ptr, const std::size_t nbytes) noexcept {
  GetKernel().Deallocate(ptr, nbytes);
}

void PushToKernelQueue(const TaskWrapper &task) {
  GetKernel().PushToQueue(task);
}

void AddModule(impl::AbstractModule *module) { GetKernel().AddModule(module); }

[[nodiscard]] int Run() { return GetKernel().Run(); }

[[nodiscard]] ThreadSignals GetThreadSignals(const ThreadId id) noexcept(
    false) {
  return GetKernel().GetThreadSignals(id);
}

bool SendKillThreadSignal(const ThreadId id) noexcept {
  return GetKernel().SendKillSignal(id);
}

bool SendSuspendThreadSignal(const ThreadId id) noexcept {
  return GetKernel().SendSuspendSignal(id);
}

bool ResumeThread(const ThreadId id) noexcept { return GetKernel().Resume(id); }

bool SuspendThisThread(const api::ThreadId *const id_hint) noexcept {
  /*
   * expected behaviour:
   * kernel_api::UnsetSignal(kThreadId, ThreadSignal::kSuspend);
   * wrapper->DeactivateThread();
   */
  return GetKernel().SuspendThisThread(id_hint);
}

bool UnsetSignal(const ThreadId id, ThreadSignal sig) noexcept {
  return GetKernel().UnsetSignal(id, sig);
}

bool DeleteThread(const ThreadId id) noexcept {
  try {
    GetKernel().DeleteThread(id);
  } catch (...) {
    return false;
  }
  return true;
}
}  // namespace kernel_api
}  // namespace api

// global scope
// global operator new and global operator delete
// FIXME::SAL warnings
[[nodiscard]] void *operator new(std::size_t count) noexcept(false) {
#if ALIGNED_ALLOCATOR_USAGE
  return api::kernel_api::Allocate(count);
#elif STL_ALLOCATOR_USAGE
  // since we overload operator new, we throw exception by ourselves
  auto ptr{std::malloc(count)};
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
  std::free(ptr);
#endif
}
// !global scope
