#include "PublicAPI.hpp"

// predefined functions
namespace api {
// from Thread.hpp (avoiding recursive #include directive)
[[nodiscard]] std::size_t GetHashedId() noexcept;
} // namespace api

namespace api {
void AddModule(impl::AbstractModule *module) { kernel_api::AddModule(module); }

[[nodiscard]] int Run() { return kernel_api::Run(); }

bool SendKillThreadSignal(const std::size_t hashed_id) noexcept {
  return kernel_api::SendKillThreadSignal(hashed_id);
}

void SendKillThreadSignal() noexcept {
  kernel_api::SendKillThreadSignal(GetHashedId());
}

bool SendSuspendThreadSignal(const std::size_t hashed_id) noexcept {
  return kernel_api::SendSuspendThreadSignal(hashed_id);
}

void SendSuspendThreadSignal() noexcept {
  kernel_api::SendSuspendThreadSignal(GetHashedId());
}

bool ResumeThread(const std::size_t hashed_id) noexcept {
  return kernel_api::ResumeThread(hashed_id);
}
} // namespace api
