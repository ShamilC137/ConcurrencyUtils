#include "AbstractModule.hpp"

namespace impl {
// ctors

AbstractModule::AbstractModule(const api::String &mid) : id_{mid} {}

// getters

api::Vector<api::String> AbstractModule::GetSlotsSignatures() const {
  api::Vector<api::String> result(slots_.size());
  std::size_t index{};
  for (const auto &pair : slots_) {
    result[index++] = pair.first;
  }
  return result;
}

// modifiers

[[nodiscard]] api::TaskWrapper
AbstractModule::TryExtractTask() noexcept(false) {
  try {
    return tasks_queue_.TryPop();
  } catch (const api::PopFailed &) {
    throw;
  } catch (...) {
    assert(false && "Panic: TryExtractTask, unexpected exception");
    throw;
  }
}
} // namespace impl
