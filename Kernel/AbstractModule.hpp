#ifndef APPLICATION_KERNEL_ABSTRACTMODULE_HPP_
#define APPLICATION_KERNEL_ABSTRACTMODULE_HPP_

// current project
#include "../API/DataStructures/Containers/HashMap.hpp"
#include "../API/DataStructures/Containers/String.hpp"
#include "../API/DataStructures/Containers/Vector.hpp"

#include "../API/DataStructures/Slot.hpp"
#include "../API/DataStructures/Task.hpp"

#include "../Config.hpp"

namespace impl {
class AbstractModule {
  // Ctors

  // Takes module identifier;
  AbstractModule(const api::String &mid);

public:
  // getters

  // Returns the module identificator
  [[nodiscard]] inline api::String GetId() const { return id_; }
  // Returns the signatures of slots
  [[nodiscard]] api::Vector<api::String> GetSlotsSignatures() const;

  // pure virtual functions
public:


  // fields
private:
  api::String id_; // module identifier; used to identify module; must be unique
  // contains slots signatures and binded handlers; signatures must be unique;
  api::HashMap<api::String, api::SlotWrapper> slots_;
};
} // namespace impl
#endif // !APPLICATION_KERNEL_ABSTRACTMODULE_HPP_
