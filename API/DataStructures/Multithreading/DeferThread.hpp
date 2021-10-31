#ifndef APPLICATION_API_DATASTRUCTURES_MULTITHREADING_DEFERTHREAD_HPP_
#define APPLICATION_API_DATASTRUCTURES_MULTITHREADING_DEFERTHREAD_HPP_
// current project
#include "../../../ImplDetails/ImplAPI/KernelAPI.hpp"
#include "../../../ImplDetails/MP/Function_types.hpp"
#include "Atomics.hpp"
#include "Thread.hpp"
#include "ThreadSignals.hpp"

// std
#include <exception>
#include <utility>

namespace api {
class DeferThread;
} // namespace api
namespace impl {
template <class ExceptionHandler, class ThreadRoutine, class... RoutineArgs>
void ThreadLaunchRoutine(api::ThreadSignals sigs, ExceptionHandler &&handler,
                         api::DeferThread *wrapper, ThreadRoutine &&routine,
                         RoutineArgs &&...args);
} // namespace impl

namespace api {
class DeferThread {
public:
  using NativeHandle = Thread::native_handle_type;
  using ID = Thread::id;

public:
  template <class ExceptionHandler, class ThreadRoutine, class... RoutineArgs>
  DeferThread(ThreadSignals sigs, ExceptionHandler &&handler,
              ThreadRoutine &&routine, RoutineArgs &&...args)
      : is_active_{},
        thread_(impl::ThreadLaunchRoutine<ExceptionHandler, ThreadRoutine,
                                          RoutineArgs...>,
                sigs, std::forward<ExceptionHandler>(handler), this,
                std::forward<ThreadRoutine>(routine),
                std::forward<RoutineArgs>(args)...) {}

  DeferThread(const DeferThread &) = delete;

  DeferThread &operator=(const DeferThread &) = delete;

  [[nodiscard]] Thread &GetAttachedThread() noexcept;

  [[nodiscard]] const Thread &GetAttachedThread() const noexcept;

  [[nodiscard]] AtomicFlag &GetIsActiveFlag() noexcept;

  [[nodiscard]] const AtomicFlag &GetIsActiveFlag() const noexcept;

  void ActivateThread();

  void DeactivateThread();

  void Join();

  void Detach();

  [[nodiscard]] NativeHandle GetNativeHandle();

  [[nodiscard]] bool Joinable() const noexcept;

  [[nodiscard]] unsigned int HardwareConcurrency() noexcept;

  [[nodiscard]] ID GetId() const noexcept;

private:
  AtomicFlag is_active_;
  Thread thread_;
};

// Returns hashed given thread id
[[nodiscard]] std::size_t GetHashedId(const DeferThread &thread) noexcept;

template <class ThreadRoutine, class... RoutineArgs>
void RoutineLoop(DeferThread *wrapper, ThreadSignals signals_mask,
                 ThreadRoutine (*routine)(RoutineArgs...),
                 RoutineArgs &&...args) {
  wrapper->GetIsActiveFlag().wait(false, api::MemoryOrder::acquire);
  if (signals_mask.Test(ThreadSignal::kExitAfterCall)) {
    routine(std::forward<RoutineArgs>(args)...);
  } else {
    // if ID is changed - UB
    const auto kHashedId{GetHashedId()};
    // current_signal have one value (i.e. ThreadSignal)
    // mb changed by main (i.e. kernel) thread
    const auto volatile &current_signal{
        kernel_api::GetThreadSignalsReference(kHashedId)};
    while (true) {
      switch (current_signal & signals_mask) {
      case ThreadSignal::kEmpty:
        routine(std::forward<RoutineArgs>(args)...);
        break;
      case ThreadSignal::kExit:
        kernel_api::DeleteThread(kHashedId);
        return;
      case ThreadSignal::kSuspend:
        kernel_api::UnsetSignal(kHashedId, ThreadSignal::kSuspend);
        wrapper->DeactivateThread();
        break;
      default:
        assert(false && "Unhandler thread signal");
      }
    }
  }
}
} // namespace api

namespace impl {
template <class ExceptionHandler, class ThreadRoutine, class... RoutineArgs>
void ThreadLaunchRoutine(api::ThreadSignals sigs, ExceptionHandler &&handler,
                         api::DeferThread *wrapper, ThreadRoutine &&routine,
                         RoutineArgs &&...args) {
  using HandlerArguments =
      typename api::Components<std::decay_t<ExceptionHandler>>::ParametersTypes;
  static_assert(HandlerArguments::size < 2u, "Too many handler arguments");

  try {
    api::RoutineLoop(wrapper, sigs, routine,
                     std::forward<RoutineArgs>(args)...);
  } catch (std::exception &ex) {
    if constexpr (HandlerArguments::size == 1u) {
      handler(static_cast<api::TypeExtractor<0u, HandlerArguments>>(ex));
    } else {
      handler();
    }
  } catch (...) {
    // Unhandled exception error
  }
}
} // namespace impl
#endif // APPLICATION_API_DATASTRUCTURES_MULTITHREADING_DEFERTHREAD_HPP_
