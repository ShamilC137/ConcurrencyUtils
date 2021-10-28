#ifndef APPLICATION_API_DATASTRUCTURES_MULTITHREADING_DEFERTHREAD_HPP_
#define APPLICATION_API_DATASTRUCTURES_MULTITHREADING_DEFERTHREAD_HPP_
// current project
#include "../../../ImplDetails/MP/Function_types.hpp"
#include "Atomics.hpp"
#include "Thread.hpp"

// std
#include <exception>

namespace api {
class DeferThread;
template <class ExceptionHandler, class ThreadRoutine, class... RoutineArgs>
void ThreadLaunchRoutine(ExceptionHandler &&handler, DeferThread *wrapper,
                         ThreadRoutine &&routine, RoutineArgs &&...args);

class DeferThread {
public:
  template <class ExceptionHandler, class ThreadRoutine, class... RoutineArgs>
  DeferThread(ExceptionHandler &&handler, ThreadRoutine &&routine,
              RoutineArgs &&...args)
      : is_active_{},
        thread_(ThreadLaunchRoutine<ExceptionHandler, ThreadRoutine,
                                    RoutineArgs...>,
                std::forward<ExceptionHandler>(handler), this,
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

template <class ExceptionHandler, class ThreadRoutine, class... RoutineArgs>
void ThreadLaunchRoutine(ExceptionHandler &&handler, DeferThread *wrapper,
                         ThreadRoutine &&routine, RoutineArgs &&...args) {
  wrapper->GetIsActiveFlag().wait(false, api::MemoryOrder::acquire);
  using HandlerArguments =
      typename Components<std::decay_t<ExceptionHandler>>::ParametersTypes;
  static_assert(HandlerArguments::size < 2u, "Too many handler arguments");

  try {
    routine(std::forward<RoutineArgs>(args)...);
  } catch (std::exception &ex) {
    if constexpr (HandlerArguments::size == 1u) {
      handler(static_cast<TypeExtractor<0u, HandlerArguments>>(ex));
    } else {
      handler();
    }
  } catch (...) {
    // Unhandled exception error
  }
}
} // namespace api
#endif // APPLICATION_API_DATASTRUCTURES_MULTITHREADING_DEFERTHREAD_HPP_
