#ifndef APPLICATION_IMPLDETAILS_IMPLAPI_KERNELAPI_HPP_
#define APPLICATION_IMPLDETAILS_IMPLAPI_KERNELAPI_HPP_

// current project
#include "../../API/DataStructures/Multithreading/ThreadSignals.hpp"
#include "../../API/DataStructures/TaskWrapper.hpp"
#include "../../Config.hpp"
#include "../AbstractModule.hpp"
#include "../../API/MemoryManagementUtilities.hpp"

#include "Errors.hpp"

// STL
#include <cstddef>

// OS dependent headers
#if WINDOWS32 | WINDOWS64
#include <Windows.h>
#else
static_assert(false, "Unsupported OS");
#endif // !WINODOWS32 | WINDOWS64

namespace kernel {
class Kernel; // singleton
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
[[nodiscard]] const api::ThreadSignals volatile&
GetThreadSignalsReference(const std::size_t id);
} // namespace kernel_api
} // namespace api

#endif // !APPLICATION_IMPLDETAILS_IMPLAPI_KERNELAPI_HPP_
