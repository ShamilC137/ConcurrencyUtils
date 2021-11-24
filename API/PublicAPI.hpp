#ifndef APPLICATION_API_PUBLICAPI_HPP_
#define APPLICATION_API_PUBLICAPI_HPP_

// STL
#include <cstddef>

// current project
#include "../ImplDetails/AbstractModule.hpp"
#include "../ImplDetails/ImplAPI/KernelAPI.hpp"
#include "../ImplDetails/Utility.hpp"
#include "DataStructures/Containers/String.hpp"
#include "DataStructures/TaskWrapper.hpp"

/// <summary>
///   This file contains function that could be used in users' scope.
/// </summary>

namespace api {
// throws: api::Deadlock

/// <summary>
///   Signal emit function.
///   All template parameters must be explicitly specified. Nonvoid return type
///   specialization. Return value can be obtained with GetResult().
/// </summary>
/// <typeparam name="ReturnType">
///   Task targer function return type
/// </typeparam>
/// <typeparam name="...Args">
///   Task targer function parameters
/// </typeparam>
/// <param name="signal_sig">
///   Caused signal full signature (i.e. with module id)
/// </param>
/// <param name="is_blocking_call">
///   if true, blocks caller until the task is completed.
/// </param>
/// <param name="priority">
///   Task order priority
/// </param>
/// <param name="...args">
///   Task parameters
/// </param>
/// <returns>
///   ReturnTaskWrapper to associated task
/// </returns>
/// <exception type="api::Deadlock">
///   Thrown if Wait() operation is broken
/// </exception>
template <class ReturnType, class... Args>
[[nodiscard]] std::enable_if_t<!impl::is_same<ReturnType, void>,
                               ReturnTaskWrapper<ReturnType, Args...>>
Emit(const api::String &signal_sig, bool is_blocking_call,
     TaskPriority priority,
     impl::ForceExplicitTypeT<Args>... args) noexcept(false) {
  ReturnTaskWrapper<ReturnType, Args...> mytask(
      new ReturnTask<ReturnType, Args...>(signal_sig, priority,
                                          std::forward<Args>(args)...),
      /*target slot*/ {});
  kernel_api::PushToKernelQueue(mytask);
  if (is_blocking_call) {
    mytask.GetTask()->Wait();  // throws
  }
  return mytask;
}

/// <summary>
///   Signal emit function.
///   All template parameters must be explicitly specified. Void return type
///   specialization.
/// </summary>
/// <typeparam name="ReturnType">
///   Task targer function return type
/// </typeparam>
/// <typeparam name="...Args">
///   Task targer function parameters
/// </typeparam>
/// <param name="signal_sig">
///   Caused signal full signature (i.e. with module id)
/// </param>
/// <param name="is_blocking_call">
///   if true, blocks caller until the task is completed.
/// </param>
/// <param name="priority">
///   Task order priority
/// </param>
/// <param name="...args">
///   Task parameters
/// </param>
/// <returns>
///   TaskWrapper to associated task
/// </returns>
/// <exception type="api::Deadlock">
///   Thrown if Wait() operation is broken
/// </exception>
template <class ReturnType, class... Args>
std::enable_if_t<impl::is_same<void, ReturnType>, TaskWrapper> Emit(
    const api::String &signal_sig, bool is_blocking_call, TaskPriority priority,
    impl::ForceExplicitTypeT<Args>... args) noexcept(false) {
  TaskWrapper mytask(
      new Task<Args...>(signal_sig, priority, std::forward<Args>(args)...),
      /*target slot*/ {});
  kernel_api::PushToKernelQueue(mytask);
  if (is_blocking_call) {
    mytask.GetTask()->Wait();  // throws
  }
  return mytask;
}

/// <summary>
///   Adds new module to kernel. Do not transfers ownership
/// </summary>
/// <param name="module">
///   Pointer to module
/// </param>
void AddModule(impl::AbstractModule *module);

/// <summary>
///   Application start point (i.e. runs other module such GUI and etc).
/// </summary>
/// <returns>
///   Program error status
/// </returns>
[[nodiscard]] int Run();

/// <summary>
///   Sends the "Exit" signal to the thread with the given id.
/// </summary>
/// <param name="id">
///   Targer thread id
/// </param>
/// <returns>
///   Returns true if signal was sent, otherwise false.
/// </returns>
bool SendKillThreadSignal(const ThreadId id) noexcept;

/// <summary>
///   Sends the "Exit" signal to this thread. Nothing happens if thread does not
///   belong to kernel threads table.
/// </summary>
void SendKillThreadSignal() noexcept;

/// <summary>
///   Sends the "Suspend" signal to the thread with the given id.
/// </summary>
/// <param name="id">
///   Target thread id
/// </param>
/// <returns>
///   Returns true if signal was sent, otherwise false.
/// </returns>
bool SendSuspendThreadSignal(const ThreadId id) noexcept;

/// <summary>
///   Sends the "Suspend" signal to the caller thread.
/// </summary>
void SendSuspendThreadSignal() noexcept;

/// <summary>
///   Sends the "Resume" signal to the thread with the given id.
/// </summary>
/// <param name="id">
///   Target thread id
/// </param>
/// <returns>
///   Returns true if signal was send, false otherwise.
/// </returns>
bool ResumeThread(const ThreadId id) noexcept;
}  // namespace api

#endif  // !APPLICATION_API_PUBLICAPI_HPP_
