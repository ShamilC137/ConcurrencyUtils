// As this header is one of the basic ones, with the help of which other headers
// are written, it additionally protected from recursive inclusion.
#if defined APPLICATION_API_PUBLICAPI_HPP_ & !defined MUTE_PUBLICAPI_ERROR_
#error "PublicAPI was already defined, i.e. recursive include directive\
detected. If you really understand what you doing, define\
MUTE_PUBLICAPI_ERROR_."
#endif

// This header is divided into two blocks: declaration block and definition
// block. It's done to other headers, which dependent from PublicAPI, can use
// its function and can be included in this header. Blocks are marked as
// DEFINITION BLOCK and DECLARATION BLOCK.

#ifndef APPLICATION_API_PUBLICAPI_HPP_
#define APPLICATION_API_PUBLICAPI_HPP_

// STL
#include <cstddef>
#include <type_traits>

// DECLARATION BLOCK BEGIN
namespace impl {
class BaseTask;
}
namespace api {
class TaskWrapper;

// Allocates continious memory block for the given number of objects of T type
// throws
template <class T> [[nodiscard]] T *Allocate(std::size_t count) noexcept(false);

// Deallocates continious memory block for the given number of objects of type T
// with the given pointer
template <class T> void Deallocate(T *ptr, std::size_t count) noexcept;

// signal_sig - target signal signature (FIXME: null-terminated string expected)
// is_blocking_call - if true, blocks caller until the task is completed.
// args - target function arguments.
// Return value: TaskWrapper if return type is void, otherwise ReturnTaskWrapper
// Nonvoid return type specialization. Value can be obtained with GetResult().
// throws
template <class ReturnType, class... Args>
std::enable_if_t<!std::is_same_v<ReturnType, void>,
                 ReturnTaskWrapper<ReturnType, Args...>>
Emit(const api::String &signal_sig, bool is_blocking_call,
     Args... args) noexcept(false);

// Void return type specialization
// throws
template <class ReturnType, class... Args>
std::enable_if_t<std::is_same_v<void, ReturnType>, TaskWrapper>
Emit(const api::String &signal_sig, bool is_blocking_call,
     Args... args) noexcept(false);

} // namespace api
// DECLARATION BLOCK END

// All headers in this module recursively including PublicAPI
#define MUTE_PUBLICAPI_ERROR_
#include "DataStructures/Containers/String.hpp"
#include "DataStructures/TaskWrapper.hpp" // fucking fuck

#define MUTE_KERNELAPI_ERROR_
#include "../ImplDetails/ImplAPI/KernelAPI.hpp"
#undef MUTE_KERNELAPI_ERROR_

#undef MUTE_PUBLICAPI_ERROR_

// DEFENITION BLOCK BEGIN
namespace api {
template <class T>
[[nodiscard]] T *Allocate(std::size_t count) noexcept(false) {
  return static_cast<T *>(
      kernel_api::Allocate(alignof(T), sizeof(T) * count)); // throws
}

template <class T> void Deallocate(T *ptr, std::size_t count) noexcept {
  kernel_api::Deallocate(ptr, sizeof(T) * count);
}

// Nonvoid return type specialization.
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
// DEFENITION BLOCK END

#endif // !APPLICATION_API_PUBLICAPI_HPP_
