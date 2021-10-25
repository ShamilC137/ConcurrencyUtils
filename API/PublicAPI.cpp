#include "PublicAPI.hpp"

namespace api {
void AddModule(impl::AbstractModule *module) { kernel_api::AddModule(module); }

[[nodiscard]] int Run() { return kernel_api::Run(); }
} // namespace api
