#ifndef APPLICATION_API_DATASTRUCTURES_SCOPEDSLOTWRAPPER_HPP_
#define APPLICATION_API_DATASTRUCTURES_SCOPEDSLOTWRAPPER_HPP_

// current project
#include "../../ImplDetails/SlotDetails.hpp"
#include "../../Config.hpp"

namespace api {
// Delete underlying slot when wrapper out of scope
class ScopedSlotWrapper {
public:
  using PointerType = impl::BaseSlot *;

  ScopedSlotWrapper(const PointerType &slot) noexcept;
  
  ScopedSlotWrapper(ScopedSlotWrapper &&rhs) noexcept;

  ScopedSlotWrapper &operator=(ScopedSlotWrapper &&rhs) noexcept;

  [[nodiscard]] inline PointerType GetSlot() noexcept { return slot_; }
  
  [[nodiscard]] inline const PointerType GetSlot() const noexcept { return slot_; }
  
  ~ScopedSlotWrapper() noexcept;

private:
  PointerType slot_;
};
} // namespace api
#endif // !APPLICATION_API_DATASTRUCTURES_SCOPEDSLOTWRAPPER_HPP_