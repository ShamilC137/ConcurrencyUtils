#ifndef APPLICATION_IMPLDETAILS_IMPLAPI_KERNELAPI_HPP_
#define APPLICATION_IMPLDETAILS_IMPLAPI_KERNELAPI_HPP_

// current project
#include "../../API/DataStructures/Multithreading/Thread.hpp"
#include "../../API/DataStructures/Multithreading/ThreadSignals.hpp"
#include "../../API/DataStructures/TaskWrapper.hpp"
#include "../../Config.hpp"
#include "../AbstractModule.hpp"
#include "Errors.hpp"

// STL
#include <cstddef>

// OS dependent headers
#if WINDOWS32 | WINDOWS64
#include <Windows.h>
#else
static_assert(false, "Unsupported OS");
#endif  // !WINODOWS32 | WINDOWS64

/// <summary>
///   This file contains the kernel possibilities that the kernel provides to
///   the user.
/// </summary>

namespace kernel {
class Kernel;  // singleton
}

namespace api {
// resolve recursive include
class DeferThread;
class DeferThreadWrapper;
namespace kernel_api {
/// <summary>
///   Creates kernel if it has not already been created and returns reference
///   to created object.
/// </summary>
/// <returns>
///   reference to kernel
/// </returns>
[[nodiscard]] inline kernel::Kernel &GetKernel() noexcept;

/// <summary>
///   Adds new task to kernel queue. Takes task (as wrapped one). Potentially
///   blocks caller thread if queue is busy.
/// </summary>
/// <param name="task">
///   Wrapped task
/// </param>
void PushToKernelQueue(const api::TaskWrapper &task);

/// <summary>
///   Adds new module to kernel. Do not transfers ownership
/// </summary>
/// <param name="module">
///   Pointer to module
/// </param>
void AddModule(impl::AbstractModule *module);

/// <summary>
///   Program start point.
/// </summary>
/// <returns>
///   Program error status
/// </returns>
[[nodiscard]] int Run();

/// <summary>
///   Breaks kernel event loop. The kernel goes to the protected mode.
/// </summary>
void Exit();

/// <summary>
///   Returns thread signals by thread id. Blocks caller thread if thread
///   signals already modified by other threads.
/// </summary>
/// <param name="id"></param>
/// <returns>
///   Reference to associated thread signals.
/// </returns>
/// <exception type="std::out_of_range">
///   Thrown is thread id not belongs to kernel manipulated threads.
/// </exception>
[[nodiscard]] api::ThreadSignals GetThreadSignals(
    const api::ThreadId id) noexcept(false);

// For all functions that set thread signal flag the same rules are applied:
// The signal will be processed only at the next loop iteration, i.e.
// the routine loop will first complete the current action (if any)
// until the end, then handle the given signal (i.e. the thread will work
// for some time after sending the signal).
// These rules are applied to functions below
// vvvvvvvvvv START LINE vvvvvvvvvv

/// <summary>
///   Sets the "Exit" flag to the associated thread.
/// </summary>
/// <param name="id">
///   Target thread id
/// </param>
/// <returns>
///   True if signal was sent, otherwise false
/// </returns>
bool SendKillThreadSignal(const api::ThreadId id) noexcept;

/// <summary>
///   Sets the "Suspend" flag to the associated thread.
/// </summary>
/// <param name="id">
///   Associated thread id
/// </param>
/// <returns>
///   True if signals was send, otherwise false.
/// </returns>
bool SendSuspendThreadSignal(const api::ThreadId id) noexcept;
// ^^^^^^^^^^ STOP LINE ^^^^^^^^^^

// Functions below use synchronization primitive to suspend/resume thread.
// vvvvvvvvvv  START LINE vvvvvvvvvv

/// <summary>
///   Resumes associated thread. If thread with given id not exists, returns
///   false.
/// </summary>
/// <param name="id">
///     Associated thread id
/// </param>
/// <returns>
///   Returns true if thread was resumed, otherwise false.
/// </returns>
bool ResumeThread(const api::ThreadId id) noexcept;

/// <summary>
///   Suspends caller thread.
///   If hint is setted up then GetId() is not called.
/// </summary>
/// <param name="id_hint">
///   Thread id hint.
/// </param>
/// <returns>
///   Returns true if thread was suspended and false otherwise.
/// </returns>
bool SuspendThisThread(const api::ThreadId *const id_hint = nullptr) noexcept;
// ^^^^^^^^^^ STOP LINE ^^^^^^^^^^

/// <summary>
///   Unsets speciefied thread signal.
/// </summary>
/// <param name="id">
///   Associated thread id
/// </param>
/// <param name="sig">
///   Signal that must be unset
/// </param>
/// <returns>
///   True if signal unset and false otherwise
/// </returns>
bool UnsetSignal(const api::ThreadId id, api::ThreadSignal sig) noexcept;

/// <summary>
///   Deletes associated with given id thread. If thread does not belong to
///   kernel threads' table, nothing happens.
/// </summary>
/// <param name="id">
///   Thread id
/// </param>
/// <returns>
///   True if thread deletion request is sent and false otherwise
/// </returns>
bool DeleteThread(const api::ThreadId id) noexcept;

/// <summary>
///   Registers given thread in the kernel. Kernel takes ownership on the
///   thread.
/// </summary>
/// <param name="thread">
///   Allocated thread.
/// </param>
/// <returns>
///   Wrapper on given thread
/// </returns>
api::DeferThreadWrapper RegisterThread(api::DeferThread *thread);
}  // namespace kernel_api
}  // namespace api

#endif  // !APPLICATION_IMPLDETAILS_IMPLAPI_KERNELAPI_HPP_
