#include "Kernel.hpp"
#if STL_ALLOCATOR_USAGE
#include <new>
#endif

namespace kernel {
// NOTE: function marked as "Parallel" can be called from few threads
// and must not block other such functions, but block other call of themselfs

Kernel::~Kernel() {}

// Ctors
Kernel::Kernel() noexcept(false)
    : run{}, stub(kMMUSize) /*throws std::bad_alloc*/ {}

// FIXME: Multithreading
[[nodiscard]] void *Kernel::Allocate(const std::size_t nbytes) noexcept(false) {
  if (exit_flag_.test()) {
    return nullptr;
  }
  return stub.Allocate(nbytes);  // throws: std::bad_alloc
}

// FIXME: Multithreading
void Kernel::Deallocate(void *ptr, const size_t nbytes) noexcept {
  if (exit_flag_.test()) {
    return;
  }
  stub.Deallocate(ptr, nbytes);
}

api::DeferThreadWrapper Kernel::RegisterThread(api::DeferThread *thread) {
  if (exit_flag_.test()) {
    thread->Close();
    thread->Activate();
    thread->Join();
    delete thread;
    return {};
  }
  return thread_manager_.AddThread(thread);
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
  if (exit_flag_.test()) {
    return;
  }
  task_manager_.PushTask(task);
}

void Kernel::AddModule(impl::AbstractModule *module) {
  if (exit_flag_.test()) {
    return;
  }
  decltype(auto) added{modules_.emplace_back(ModuleDescriptor{module})};
  task_manager_.AddModule(&added);
}

// FIXME: check
void Kernel::OnExitRoutine() { thread_manager_.ForceDeleteAll(); }

// FIXME: complete
int Kernel::EventLoop() {
  while (true) {
    if (exit_flag_.test(api::MemoryOrder::acquire)) {
      OnExitRoutine();
      break;
    }

    for (unsigned char counter{}; counter < kMaxTasksPerLoop; ++counter) {
      if (!task_manager_.SendNextTask()) {
        break;
      }
    }

    if (exit_flag_.test(api::MemoryOrder::acquire)) {
      OnExitRoutine();
      break;
    }
    for (unsigned char counter{}; counter < kMaxThreadsPerLoop; ++counter) {
      if (thread_manager_.ManageClosedThread() !=
          api::OperationResult::kSuccess) {
        break;
      }
    }

    // FIXME: error handling must be here
  }
  return static_cast<int>(KernelStatus::kOk);
}

// Stuff:
// 1) Init all modules
// 2) Start all threads
// 3) Event loop:
//  3.1) Chech tasks
//  3.2) Check threads
//  3.3) Check errors
//  Need to check exit_flag before any step at the step 3.
[[nodiscard]] int Kernel::Run() {
  if (run.test_and_set()) {
    return static_cast<int>(KernelStatus::kRun);
  }

  for (auto &md : modules_) {
    if (auto status{md.module->Init()};
        status != impl::ModuleInitErrorStatus::kOk) {
      return static_cast<int>(status);
    }
  }

  thread_manager_.StartAll();

  return EventLoop();
}

void Kernel::Exit() noexcept { exit_flag_.test_and_set(); }
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

void Exit() { GetKernel().Exit(); }

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

api::DeferThreadWrapper RegisterThread(api::DeferThread *thread) {
  return GetKernel().RegisterThread(thread);
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
