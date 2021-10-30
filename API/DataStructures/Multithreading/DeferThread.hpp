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

namespace api {
class DeferThread;

template <class ThreadRoutine, class... RoutineArgs>
void RoutineLoop(ThreadSignals signals_mask,
                 ThreadRoutine (*routine)(RoutineArgs...),
                 RoutineArgs &&...args) {
  if (signals_mask.Test(ThreadSignal::kExitAfterCall)) {
    routine(std::forward<RoutineArgs>(args)...);
  } else {
    // current_signal have one value (i.e. ThreadSignal)
    const auto volatile &current_signal{
        kernel_api::GetThreadSignalsReference(GetId())};
    while (true) {
      switch (current_signal & signals_mask) {
      case ThreadSignal::kExit:
        return;
      case ThreadSignal::kSuspend:
        // FIXME: suspending thread
        break;
      default:
        routine(std::forward<RoutineArgs>(args)...);
      }
    }
  }
}
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

  [[nodiscard]] inline api::Thread &GetAttachedThread() noexcept {
    return thread_;
  }

  [[nodiscard]] inline const api::Thread &GetAttachedThread() const noexcept {
    return thread_;
  }

  [[nodiscard]] inline api::AtomicFlag &GetIsActiveFlag() noexcept {
    return is_active_;
  }

  [[nodiscard]] const api::AtomicFlag &GetIsActiveFlag() const noexcept {
    return is_active_;
  }

  void ActivateThread() {
    is_active_.test_and_set(api::MemoryOrder::release);
    is_active_.notify_one();
  }

  inline void Join() { thread_.join(); }

  inline void Detach() { thread_.detach(); }

  [[nodiscard]] inline auto GetNativeHandle() {
    return thread_.native_handle();
  }

  [[nodiscard]] inline bool Joinable() const noexcept {
    return thread_.joinable();
  }

  [[nodiscard]] inline unsigned int HardwareConcurrency() noexcept {
    return thread_.hardware_concurrency();
  }

  [[nodiscard]] inline auto GetId() const noexcept { return thread_.get_id(); }

private:
  api::AtomicFlag is_active_;
  api::Thread thread_;
};
} // namespace api

namespace impl {
template <class ExceptionHandler, class ThreadRoutine, class... RoutineArgs>
void ThreadLaunchRoutine(api::ThreadSignals sigs, ExceptionHandler &&handler,
                         api::DeferThread *wrapper, ThreadRoutine &&routine,
                         RoutineArgs &&...args) {
  wrapper->GetIsActiveFlag().wait(false, api::MemoryOrder::acquire);
  using HandlerArguments =
      typename api::Components<std::decay_t<ExceptionHandler>>::ParametersTypes;
  static_assert(HandlerArguments::size < 2u, "Too many handler arguments");

  try {
    api::RoutineLoop(sigs, routine, std::forward<RoutineArgs>(args)...);
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
