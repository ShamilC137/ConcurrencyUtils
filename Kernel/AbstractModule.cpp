#include "AbstractModule.hpp"

namespace impl {
// ctors

AbstractModule::AbstractModule(const api::String &mid) : id_{mid} {}

// getters

api::Vector<api::String> AbstractModule::GetSlotsSignatures() const {
  return {};
}
} // namespace impl
