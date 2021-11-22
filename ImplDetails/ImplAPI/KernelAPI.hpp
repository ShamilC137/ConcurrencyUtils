#ifndef APPLICATION_IMPLDETAILS_IMPLAPI_KERNELAPI_HPP_
#define APPLICATION_IMPLDETAILS_IMPLAPI_KERNELAPI_HPP_

// current project
#include "../../API/DataStructures/Multithreading/Thread.hpp"
#include "../../API/DataStructures/Multithreading/ThreadSignals.hpp"
#include "../../API/DataStructures/TaskWrapper.hpp"
#include "../../API/MemoryManagementUtilities.hpp"
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

namespace kernel {
class Kernel;  // singleton
}

namespace api {
namespace kernel_api {
// Return created kernel object; the only way to create such object
[[nodiscard]] inline kernel::Kernel &GetKernel() noexcept;

// Adds new task to kernel queue. Takes task (as wrapped one). Potentially
// blocks caller thread if queue is busy.
// Warning: if wrapper object on caller thread is a temporary object,
// task will be deleted after all slots routine complete!
void PushToKernelQueue(const api::TaskWrapper &task);

// Adds new module to kernel. Do not transfers ownership
void AddModule(impl::AbstractModule *module);

// Runs the program
[[nodiscard]] int Run();

// Returns reference to (i.e. returned value mb changed if it saved as
// reference) caller thread signals by thread id (api::GetId()).
// Its value can be used for signal handler (it may be changed from another
// thread at any moment, that's why it marked as volatile).
// Yes, there is data race, but I don't care (one writter, many readers)
[[nodiscard]] const api::ThreadSignals volatile &GetThreadSignalsReference(
    const api::ThreadId id) noexcept;

// For all functions that set thread signal flag the same rules are applied:
// The signal will be processed only at the next loop iteration, i.e.
// the routine loop will first complete the current action (if any)
// until the end, then handle the given signal (i.e. the thread will work
// for some time after sending the signal).
// These rules are applied to functions below
// vvvvvvvvvv START LINE vvvvvvvvvv

// Sets the "Exit" flag to the associated thread.
// Returns true if signal was sent, otherwise false.
bool SendKillThreadSignal(const api::ThreadId id) noexcept;

// Sets the "Suspend" flag to the associated thread.
// Returns true if signals was send, otherwise false.
bool SendSuspendThreadSignal(const api::ThreadId id) noexcept;
// ^^^^^^^^^^ STOP LINE ^^^^^^^^^^

// Functions below use synchronization primitive to suspend/resume thread.
// vvvvvvvvvv  START LINE vvvvvvvvvv

// Resumes associated thread. Returns true if thread was resumed, otherwise
// false. The thread may be suspended by "SuspendThread".
bool ResumeThread(const api::ThreadId id) noexcept;

// Suspends caller thread. Returns true if thread was suspended and false
// otherwise. If hint is setted up then GetId() is not called.
bool SuspendThisThread(const api::ThreadId *const id_hint = nullptr) noexcept;
// ^^^^^^^^^^ STOP LINE ^^^^^^^^^^

// Unsets speciefied thread signal.
void UnsetSignal(const api::ThreadId id, api::ThreadSignal sig) noexcept;

void DeleteThread(const api::ThreadId id);
}  // namespace kernel_api
}  // namespace api

#endif  // !APPLICATION_IMPLDETAILS_IMPLAPI_KERNELAPI_HPP_
