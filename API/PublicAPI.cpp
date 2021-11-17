#include "PublicAPI.hpp"

#include "DataStructures/Multithreading/Thread.hpp"

namespace api {
void AddModule(impl::AbstractModule *module) { kernel_api::AddModule(module); }

[[nodiscard]] int Run() { return kernel_api::Run(); }

bool SendKillThreadSignal(const ThreadId id) noexcept {
  return kernel_api::SendKillThreadSignal(id);
}

void SendKillThreadSignal() noexcept {
  kernel_api::SendKillThreadSignal(GetId());
}

bool SendSuspendThreadSignal(const ThreadId id) noexcept {
  return kernel_api::SendSuspendThreadSignal(id);
}

void SendSuspendThreadSignal() noexcept {
  kernel_api::SendSuspendThreadSignal(GetId());
}

bool ResumeThread(const ThreadId id) noexcept {
  return kernel_api::ResumeThread(id);
}
}  // namespace api
