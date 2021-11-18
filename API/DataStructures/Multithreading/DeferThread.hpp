#ifndef APPLICATION_API_DATASTRUCTURES_MULTITHREADING_DEFERTHREAD_HPP_
#define APPLICATION_API_DATASTRUCTURES_MULTITHREADING_DEFERTHREAD_HPP_
// current project
#include "../../../ImplDetails/ImplAPI/KernelAPI.hpp"
#include "../../../ImplDetails/MP/Components.hpp"
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
void ThreadLaunchRoutine(const bool exit_after_call_flag,
                         ExceptionHandler &&handler, api::DeferThread *wrapper,
                         ThreadRoutine &&routine, RoutineArgs &&...args);
} // namespace impl

namespace api {
class DeferThread {
public:
  using NativeHandle = Thread::native_handle_type;
  using ID = Thread::id;

public:
  template <class ExceptionHandler, class ThreadRoutine, class... RoutineArgs>
  DeferThread(const bool exit_after_call_flag, ExceptionHandler &&handler,
              ThreadRoutine &&routine, RoutineArgs &&...args)
      : is_active_{},
        thread_(impl::ThreadLaunchRoutine<ExceptionHandler, ThreadRoutine,
                                          RoutineArgs...>,
                exit_after_call_flag, std::forward<ExceptionHandler>(handler),
                this, std::forward<ThreadRoutine>(routine),
                std::forward<RoutineArgs>(args)...) {}

  DeferThread(const DeferThread &) = delete;

  DeferThread &operator=(const DeferThread &) = delete;

  [[nodiscard]] Thread &GetAttachedThread() noexcept;

  [[nodiscard]] const Thread &GetAttachedThread() const noexcept;

  [[nodiscard]] AtomicFlag &GetIsActiveFlag() noexcept;

  [[nodiscard]] const AtomicFlag &GetIsActiveFlag() const noexcept;

  void ActivateThread(api::MemoryOrder order = api::MemoryOrder::release);

  void DeactivateThread(api::MemoryOrder order = api::MemoryOrder::acquire);

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

template <class ThreadRoutine, class... RoutineArgs>
void RoutineLoop(DeferThread *wrapper, const bool exit_after_call_flag,
                 ThreadRoutine (*routine)(RoutineArgs...),
                 RoutineArgs &&...args) {
  if (exit_after_call_flag) {
    routine(std::forward<RoutineArgs>(args)...);
  } else {
    // if ID is changed - UB
    const auto kThreadId{GetId()};
    // current_signal have one value (i.e. ThreadSignal)
    // mb changed by main (i.e. kernel) thread
    const auto volatile &thread_signal{
        kernel_api::GetThreadSignalsReference(kThreadId)};
    while (true) {
      ThreadSignal current_signal{static_cast<ThreadSignal>(thread_signal)};
      switch (current_signal) {
      case ThreadSignal::kEmpty:
        routine(std::forward<RoutineArgs>(args)...);
        break;
      case ThreadSignal::kExit:
        kernel_api::DeleteThread(kThreadId);
        return;
      case ThreadSignal::kSuspend:
        kernel_api::SuspendThisThread(&kThreadId);
        break;
      default:
        assert(false && "Unhandled thread signal");
      }
    }
  }
}
} // namespace api

namespace impl {
namespace impl_details {
template <class T> struct TypeResolution {
  using Arguments = typename api::Components<std::decay_t<T>>::ParametersTypes;
  constexpr static std::size_t size{Arguments::size};
};

struct Sizer {
  constexpr static std::size_t size{0};
};

template <> struct TypeResolution<decltype(nullptr)> {
  using Arguments = Sizer;
};

template <class Callee> struct CallMaker {
  static void Call(Callee &&callee) { callee(); }
};

template <> struct CallMaker<decltype(nullptr)> {
  static void Call(void *) { /*do nothing*/
  }
};
} // namespace impl_details

template <class ExceptionHandler, class ThreadRoutine, class... RoutineArgs>
void ThreadLaunchRoutine(const bool exit_after_call_flag,
                         ExceptionHandler &&handler, api::DeferThread *wrapper,
                         ThreadRoutine &&routine, RoutineArgs &&...args) {
  wrapper->GetIsActiveFlag().wait(false, api::MemoryOrder::acquire);
  using HandlerArguments =
      typename impl_details::TypeResolution<ExceptionHandler>::Arguments;
  static_assert(HandlerArguments::size < 2u, "Too many handler arguments");

  try {
    api::RoutineLoop(wrapper, exit_after_call_flag, routine,
                     std::forward<RoutineArgs>(args)...);
  } catch (std::exception &ex) {
    if constexpr (HandlerArguments::size == 1u) {
      handler(static_cast<api::TypeExtractor<0u, HandlerArguments>>(ex));
    } else {
      impl_details::CallMaker<ExceptionHandler>::Call(
          std::forward<ExceptionHandler>(handler));
    }
  } catch (...) {
    // Unhandled exception error
  }
}
} // namespace impl
#endif // APPLICATION_API_DATASTRUCTURES_MULTITHREADING_DEFERTHREAD_HPP_
