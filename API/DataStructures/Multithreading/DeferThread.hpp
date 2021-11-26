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

/// <summary>
///   This file contains DeferThread class that wraps Thread and all its
///   helpers.
/// </summary>

namespace api {
class DeferThread;
}  // namespace api
namespace impl {
/// <summary>
///   Controls given thread function. Sets exception handler (if exception
///   inherits from std::exception). Handles all thread signals.
/// </summary>
/// <typeparam name="ExceptionHandler">
///   Exception handler type
/// </typeparam>
/// <typeparam name="ThreadRoutine">
///   Thread routine function type
/// </typeparam>
/// <typeparam name="...RoutineArgs">
///   Thread routine parameters types
/// </typeparam>
/// <param name="call_once_flag">
///   If true, thread exitting after function call and runs in loop otherwise.
/// </param>
/// <param name="handler">
///   Exceptiong handler
/// </param>
/// <param name="wrapper">
///   Pointer to associated thread
/// </param>
/// <param name="routine">
///   Thread routine function
/// </param>
/// <param name="...args">
///   Thread function arguments
/// </param>
template <class ExceptionHandler, class ThreadRoutine, class... RoutineArgs>
void ThreadLaunchRoutine(const bool call_once_flag, ExceptionHandler &&handler,
                         api::DeferThread *wrapper, ThreadRoutine &&routine,
                         RoutineArgs &&...args);
}  // namespace impl

namespace api {
/// <summary>
///   Wraps Thread class. Provides two thing:
///   1) defer thread activation
///   2) endless loop which will call given function if no signals set.
/// </summary>
class DeferThread {
 public:
  using NativeHandle = Thread::native_handle_type;
  using ID = Thread::id;

 public:
  /// <typeparam name="ExceptionHandler">
  ///   Exception handler type
  /// </typeparam>
  /// <typeparam name="ThreadRoutine">
  ///   Thread routine function type
  /// </typeparam>
  /// <typeparam name="...RoutineArgs">
  ///   Thread routine parameters types
  /// </typeparam>
  /// <param name="call_once_flag">
  ///   If true, thread exitting after function call and runs in loop otherwise.
  /// </param>
  /// <param name="handler">
  ///   Exceptiong handler
  /// </param>
  /// <param name="wrapper">
  ///   Pointer to associated thread
  /// </param>
  /// <param name="routine">
  ///   Thread routine function
  /// </param>
  /// <param name="...args">
  ///   Thread function arguments
  /// </param>
  template <class ExceptionHandler, class ThreadRoutine, class... RoutineArgs>
  DeferThread(const bool exit_after_call_flag, ExceptionHandler &&handler,
              ThreadRoutine &&routine, RoutineArgs &&...args)
      : is_active_{},
        is_closed_{},
        signals_{},
        nreferences_{},
        thread_(impl::ThreadLaunchRoutine<ExceptionHandler, ThreadRoutine,
                                          RoutineArgs...>,
                exit_after_call_flag, std::forward<ExceptionHandler>(handler),
                this, std::forward<ThreadRoutine>(routine),
                std::forward<RoutineArgs>(args)...) {}

  DeferThread(const DeferThread &) = delete;

  DeferThread &operator=(const DeferThread &) = delete;

  /// <returns>
  ///   Associated with this DeferThread Thread object
  /// </returns>
  [[nodiscard]] Thread &GetAttachedThread() noexcept;

  /// <returns>
  ///   Associated with this DeferThread Thread object
  /// </returns>
  [[nodiscard]] const Thread &GetAttachedThread() const noexcept;

  /// <returns>
  ///   Associated with this DeferThread flag
  /// </returns>
  [[nodiscard]] AtomicFlag &GetIsActiveFlag() noexcept;

  /// <returns>
  ///   Associated with this DeferThread flag
  /// </returns>
  [[nodiscard]] const AtomicFlag &GetIsActiveFlag() const noexcept;

  /// <summary>
  ///   Activates all suspended on associated with this DeferThread flag
  ///   threads.
  /// </summary>
  /// <param name="order">
  ///   test_and_set() memory order
  /// </param>
  void Activate(api::MemoryOrder order = api::MemoryOrder::release);

  /// <summary>
  ///   Deactivates caller thread (i.e. pushes it to wait() queue)
  /// </summary>
  /// <param name="order">
  ///   wait() memory order
  /// </param>
  void DeactivateCallerThread(
      api::MemoryOrder order = api::MemoryOrder::acquire);

  void Join();

  void Detach();

  [[nodiscard]] NativeHandle GetNativeHandle();

  [[nodiscard]] bool Joinable() const noexcept;

  [[nodiscard]] unsigned int HardwareConcurrency() noexcept;

  [[nodiscard]] ID GetId() const noexcept;

  /// <returns>
  ///   Current thread signals
  /// </returns>
  [[nodiscard]] ThreadSignals GetSignals() noexcept;

  /// <summary>
  ///   Sets given signal
  /// </summary>
  /// <param name="signal">
  ///   Signal to set
  /// </param>
  void SetSignal(ThreadSignal signal) noexcept;

  /// <summary>
  ///   Unsets given signal
  /// </summary>
  /// <param name="signal">
  ///   Signal to unset
  /// </param>
  void UnsetSignal(ThreadSignal signal) noexcept;

  /// <summary>
  ///   Closes this thread. Closed thread is a thread which treated as joined
  ///   thread by DeferThreadWrapper. Sets "Exit" signal to underlying thread.
  /// </summary>
  void Close() noexcept;

  /// <returns>
  ///   Close state
  /// </returns>
  [[nodiscard]] bool IsClosed() const volatile noexcept;

 private:
  friend class DeferThreadWrapper;
  /// <summary>
  ///   Increments number of DeferThreadWrapper object referenced on this
  ///   object.
  /// </summary>
  /// <param name="order">
  ///   add() memory order
  /// </param>
  /// <returns>
  ///   If thread is closed, returns 0, otherwise returns new number of
  ///   references.
  /// </returns>
  [[nodiscard]] unsigned char IncrementNumberOfReferences(
      api::MemoryOrder order = api::MemoryOrder::relaxed) noexcept;

  /// <summary>
  ///   Decrements number of DeferThreadWrapper object referenced on this
  ///   object.
  /// </summary>
  /// <param name="order">
  ///   sub() memory order
  /// </param>
  /// <returns>
  ///   New number of references
  /// </returns>
  unsigned char DecrementNumberOfReferences(
      api::MemoryOrder order = api::MemoryOrder::relaxed) noexcept;

 public:
  /// <returns>
  ///   Returns number of current DeferThreadWrapper objects referenced on this
  ///   object.
  /// </returns>
  [[nodiscard]] unsigned char NumberOfReferences(
      api::MemoryOrder order = api::MemoryOrder::acquire) const noexcept;

 private:
  AtomicFlag is_active_;
  Thread thread_;
  volatile bool is_closed_;  // if associated thread closed; guarded by mutex
  Mutex close_mutex_;
  Atomic<ThreadSignals::Type> signals_;
  Atomic<unsigned char> nreferences_;
};

/// <summary>
///   Provides endless loop which handles all thread signals associated with its
///   thread.
/// </summary>
template <class ThreadRoutine, class... RoutineArgs>
void RoutineLoop(const bool exit_after_call_flag,
                 ThreadRoutine (*routine)(RoutineArgs...),
                 RoutineArgs &&...args) {
  if (exit_after_call_flag) {
    routine(std::forward<RoutineArgs>(args)...);
  } else {
    // if ID is changed - UB
    const auto kThreadId{GetId()};
    // current_signal have one value (i.e. ThreadSignal)
    // mb changed by main (i.e. kernel) thread
    while (true) {
      auto current_signal{
          static_cast<ThreadSignal>(kernel_api::GetThreadSignals(kThreadId))};
      switch (current_signal) {
        case ThreadSignal::kExit:
          kernel_api::DeleteThread(kThreadId);
          return;
        case ThreadSignal::kSuspend:
          kernel_api::UnsetSignal(kThreadId, ThreadSignal::kSuspend);
          kernel_api::SuspendThisThread(&kThreadId);
          break;
        case ThreadSignal::kEmpty:
          routine(std::forward<RoutineArgs>(args)...);
          break;
        default:
          assert(false && "Unhandled thread signal");
      }
    }
  }
}
}  // namespace api

namespace impl {
namespace impl_details {
template <class T>
struct TypeResolution {
  using Arguments = typename api::Components<std::decay_t<T>>::ParametersTypes;
  constexpr static std::size_t size{Arguments::size};
};

struct Sizer {
  constexpr static std::size_t size{0};
};

template <>
struct TypeResolution<decltype(nullptr)> {
  using Arguments = Sizer;
};

template <class Callee>
struct CallMaker {
  static void Call(Callee &&callee) { callee(); }
};

template <>
struct CallMaker<decltype(nullptr)> {
  static void Call(void *) { /*do nothing*/
  }
};
}  // namespace impl_details

template <class ExceptionHandler, class ThreadRoutine, class... RoutineArgs>
void ThreadLaunchRoutine(const bool exit_after_call_flag,
                         ExceptionHandler &&handler, api::DeferThread *wrapper,
                         ThreadRoutine &&routine, RoutineArgs &&...args) {
  wrapper->GetIsActiveFlag().wait(false, api::MemoryOrder::acquire);
  using HandlerArguments =
      typename impl_details::TypeResolution<ExceptionHandler>::Arguments;
  static_assert(HandlerArguments::size < 2u, "Too many handler arguments");

  try {
    api::RoutineLoop(exit_after_call_flag, routine,
                     std::forward<RoutineArgs>(args)...);
    wrapper->Close();
  } catch (std::exception &ex) {
    wrapper->Close();
    if constexpr (HandlerArguments::size == 1u) {
      handler(static_cast<api::TypeExtractor<0u, HandlerArguments>>(ex));
    } else {
      impl_details::CallMaker<ExceptionHandler>::Call(
          std::forward<ExceptionHandler>(handler));
    }
  } catch (...) {
    wrapper->Close();
    // Unhandled exception error
  }
}
}  // namespace impl
#endif  // APPLICATION_API_DATASTRUCTURES_MULTITHREADING_DEFERTHREAD_HPP_
