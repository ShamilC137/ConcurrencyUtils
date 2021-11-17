#ifndef APPLICATION_API_PUBLICAPI_HPP_
#define APPLICATION_API_PUBLICAPI_HPP_

// STL
#include <cstddef>
#include <type_traits>

// current project
#include "DataStructures/Containers/String.hpp"
#include "DataStructures/TaskWrapper.hpp"
#include "MemoryManagementUtilities.hpp"

#include "../ImplDetails/AbstractModule.hpp"
#include "../ImplDetails/ImplAPI/KernelAPI.hpp"

namespace api {
// signal_sig - target signal signature
// is_blocking_call - if true, blocks caller until the task is completed.
// args - target function arguments.
// Return value: ReturnTaskWrapper
// All template parameters must be explicitly specified.
// Nonvoid return type specialization. Value can be obtained with GetResult().
// throws: api::Deadlock
template <class ReturnType, class... Args>
[[nodiscard]] std::enable_if_t<!std::is_same_v<ReturnType, void>,
                               ReturnTaskWrapper<ReturnType, Args...>>
Emit(const api::String &signal_sig, bool is_blocking_call,
     TaskPriority priority,
     impl::ForceExplicitTypeT<Args>... args) noexcept(false) {
  ReturnTaskWrapper<ReturnType, Args...> mytask(
      new ReturnTask<ReturnType, Args...>(signal_sig, priority,
                                          std::forward<Args>(args)...),
      {});
  kernel_api::PushToKernelQueue(mytask);
  if (is_blocking_call) {
    mytask.GetTask()->Wait(); // throws
  }
  return mytask;
}

// signal_sig - target signal signature
// is_blocking_call - if true, blocks caller until the task is completed.
// args - target function arguments.
// Return value: TaskWrapper
// All template parameters must be explicitly specified.
// Void return type specialization
// throws: api::Deadlock
template <class ReturnType, class... Args>
std::enable_if_t<std::is_same_v<void, ReturnType>, TaskWrapper>
Emit(const api::String &signal_sig, bool is_blocking_call,
     TaskPriority priority,
     impl::ForceExplicitTypeT<Args>... args) noexcept(false) {
  TaskWrapper mytask(
      new Task<Args...>(signal_sig, priority, std::forward<Args>(args)...), {});
  kernel_api::PushToKernelQueue(mytask);
  if (is_blocking_call) {
    mytask.GetTask()->Wait(); // throws
  }
  return mytask;
}

// Adds new module to kernel. Do not transfers ownership
void AddModule(impl::AbstractModule *module);

// Runs the program. Returns error status
[[nodiscard]] int Run();

// Sends the "Exit" signal to the thread with the given id.
// Returns true if signal was sent, otherwise false.
bool SendKillThreadSignal(const ThreadId id) noexcept;

// Sends the "Exit" signal to the caller thread.
void SendKillThreadSignal() noexcept;

// Sends the "Suspend" signal to the thread with the given id.
// Returns true if signal was sent, otherwise false.
bool SendSuspendThreadSignal(const ThreadId id) noexcept;

// Sends the "Suspend" signal to the caller thread.
void SendSuspendThreadSignal() noexcept;

// Sends the "Resume" signal to the thread with the given id.
// Returns true if signal was send, false otherwise.
bool ResumeThread(const ThreadId id) noexcept;
} // namespace api

#endif // !APPLICATION_API_PUBLICAPI_HPP_
