#ifndef APPLICATION_KERNEL_ABSTRACTMODULE_HPP_
#define APPLICATION_KERNEL_ABSTRACTMODULE_HPP_

// current project
#include "../API/DataStructures/Containers/HashMap.hpp"
#include "../API/DataStructures/Containers/String.hpp"
#include "../API/DataStructures/Containers/Vector.hpp"

#include "../API/DataStructures/Slot.hpp"
#include "../API/DataStructures/Task.hpp"
#include "../API/DataStructures/TaskWrapper.hpp"
#include "../API/DataStructures/ScopedSlotWrapper.hpp"

#include "../Config.hpp"

namespace impl {
class AbstractModule {
  // Ctors
public:
  // Takes module identifier;
  AbstractModule(const api::String &mid);

  // getters
public:
  // Returns the module identificator
  [[nodiscard]] inline const api::String &GetId() const noexcept { return id_; }
  // Returns the signatures of slots
  [[nodiscard]] api::Vector<api::String> GetSlotsSignatures() const;

  // modifying functions
public:
  // Slot must be created on heap. Module takes ownership on slot.
  void AddSlotToTable(const api::String &slot_sig, impl::BaseSlot *slot);

  // pure virtual functions
public:
  // Module initialization consists of few steps:
  // 1) Objects creation
  // 2) Main thread creation
  // 3) Probably additional threads creation
  // 4) Fills slots table (i.e. slots signatures -> slot)
  // Returns true if module was initialized successfully, false otherwise.
  virtual bool Init() = 0;

  // fields
private:
  api::String id_; // module identifier; used to identify module; must be unique
  // contains slots signatures and binded handlers; signatures must be unique;
  api::HashMap<api::String, api::ScopedSlotWrapper> slots_;
};
} // namespace impl
#endif // !APPLICATION_KERNEL_ABSTRACTMODULE_HPP_
