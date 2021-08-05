#ifndef APPLICATION_API_PUBLICAPI_HPP_
#define APPLICATION_API_PUBLICAPI_HPP_

// STL
#include <cstddef>
#include <type_traits>

// current project
#include "DataStructures/Containers/String.hpp"
#include "DataStructures/TaskWrapper.hpp"
#include "MemoryManagementUtilities.hpp"

#include "../ImplDetails/ImplAPI/KernelAPI.hpp"

namespace api {
// signal_sig - target signal signature
// is_blocking_call - if true, blocks caller until the task is completed.
// args - target function arguments.
// Return value: TaskWrapper if return type is void, otherwise ReturnTaskWrapper
// Nonvoid return type specialization. Value can be obtained with GetResult().
// throws
template <class ReturnType, class... Args>
[[nodiscard]] std::enable_if_t<!std::is_same_v<ReturnType, void>,
                               ReturnTaskWrapper<ReturnType, Args...>>
Emit(const api::String &signal_sig, bool is_blocking_call,
     Args... args) noexcept(false) {
#if ALIGNED_ALLOCATOR_USAGE
  ReturnTaskWrapper<ReturnType, Args...> mytask(
      Allocate<ReturnTask<ReturnType, Args...>>(1));
  new (mytask.GetTask())
      ReturnTask<ReturnType, Args...>(signal_sig, std::forward<Args>(args)...);
#elif STL_ALLOCATOR_USAGE
  ReturnTaskWrapper<ReturnType, Args...> mytask(
      new ReturnTask<ReturnType, Args...>(signal_sig,
                                          std::forward<Args>(args)...));
#endif
  kernel_api::PushToQueue(mytask);
  if (is_blocking_call) {
    mytask.GetTask()->Wait(); // throws
  }
  return mytask;
}

// Void return type specialization
// throws
template <class ReturnType, class... Args>
std::enable_if_t<std::is_same_v<void, ReturnType>, TaskWrapper>
Emit(const api::String &signal_sig, bool is_blocking_call,
     Args... args) noexcept(false) {
#if STL_ALLOCATOR_USAGE
  TaskWrapper mytask(new Task<Args...>(signal_sig, is_blocking_call,
                                       std::forward<Args>(args)...));
#elif ALIGNED_ALLOCATOR_USAGE
  TaskWrapper mytask(Allocate<Task<Args...>>(1));
  new (mytask.GetTask()) 
    Task<Args...>(signal_sig, is_blocking_call, std::forward<Args>(args)...));
#endif
  kernel_api::PushToQueue(mytask);
  if (is_blocking_call) {
    mytask.GetTask()->Wait(); // throws
  }
  return mytask;
}
} // namespace api

#endif // !APPLICATION_API_PUBLICAPI_HPP_
