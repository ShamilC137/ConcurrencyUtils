#ifndef APPLICATION_IMPLDETAILS_ABSTRACTMODULE_HPP_
#define APPLICATION_IMPLDETAILS_ABSTRACTMODULE_HPP_

// current project
#include "../API/DataStructures/Containers/HashMap.hpp"
#include "../API/DataStructures/Containers/String.hpp"
#include "../API/DataStructures/Containers/Vector.hpp"

#include "../API/DataStructures/Multithreading/Atomics.hpp"
#include "../API/DataStructures/Multithreading/UnboundedPriorityBlockingQueue.hpp"

#include "../API/DataStructures/ScopedSlotWrapper.hpp"
#include "../API/DataStructures/Slot.hpp"
#include "../API/DataStructures/Task.hpp"
#include "../API/DataStructures/TaskWrapper.hpp"

#include "../Config.hpp"

#include "ImplAPI/Errors.hpp"

// STD
#include <cassert>
#include <memory> // allocator traits

namespace api {
// indicates that slot call must be done whether it already called or not
struct ForceSlotCall {};
} // namespace api

namespace impl {
// AbstractModule is the basic module for other program modules. Program
// module is used to extend application functionality. AbstactModule contains
// all features that all modules must contain. AbstactModule provide functions
// for working with tasks queue and slots
class AbstractModule {
  // aliases
public:
  template <class T> using Allocator = std::allocator<T>;

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
  // Adds new slot to slots' table.
  // Slot must be created on heap.
  // Module takes ownership on slot.
  inline void AddSlotToTable(const api::String &slot_sig,
                             impl::BaseSlot *slot) {
    const auto is_inserted{slots_.try_emplace(slot_sig, slot).second};
    assert(is_inserted && "Panic: AddSlotToTable, key already exists");
  }

  // Extracts a new task from the tasks queue. Potentially block until
  // a new task appears or queue is modified.
  [[nodiscard]] inline api::TaskWrapper ExtractTask() noexcept(false) {
    return tasks_queue_.Pop();
  }

  // Extracts a new task from the tasks queue. Return ExtractionPair that
  // contains two things: status flag and TaskWrapper. If extraction is
  // failed then status flag becomes false and TaskWrapper contains nullptr.
  // Otherwise, status flag becomes true and TaskWrapper contains correct task.
  // Do not block caller thread if queue is empty or if queue busy.
  // Throws if extraction is failed: api::PopFailed.
  [[nodiscard]] api::TaskWrapper TryExtractTask() noexcept(false);

  // Pushes a new task to the tasks queue. Potentially blocks if queue is
  // modified.
  inline void PushTask(api::TaskWrapper task) { tasks_queue_.Push(task); }

  // Pushes a new task to the task queue. Not blocks caller thread. Returns
  // true if push operation is completed, otherwise false.
  inline bool TryPushTask(api::TaskWrapper task) {
    return tasks_queue_.TryPush(task);
  }

  // Extract task from tasks queue and execute it. Potentially blocks caller
  // thread if queue is modified or empty or slot have priority. Only one
  // call to underlying slot is allowed.
  // Throws: std::out_of_range, api::BadSlotCall, api::PopFailed
  // Return value: Returns kOk if called was completed successfully and kBusy
  // if slot already called.
  ThreadResourceErrorStatus ExecuteTask() noexcept(false);

  // Extract task from tasks queue and execute it. Potentially blocks caller
  // thread if queue is modified or empty or slot have priority. Makes call
  // whether slot busy or not.
  // Throws: std::out_of_range, api::BadSlotCall, api::PopFailed
  void ExecuteTask(api::ForceSlotCall) noexcept(false);

  // Takes task and execute it. Potentially blocks caller thread if slot have
  // priority.
  // Throws: std::out_of_range, api::BadSlotCall
  // Return value: Returns kOk if called was completed successfully and kBusy
  // if slot already called.
  ThreadResourceErrorStatus ExecuteTask(api::TaskWrapper task) noexcept(false);

  // Extract task from tasks queue and execute it. Potentially blocks caller
  // thread if queue is modified or empty or slot have priority. Makes call
  // whether slot busy or not.
  // Throws: std::out_of_range, api::BadSlotCall
  void ExecuteTask(api::TaskWrapper task, api::ForceSlotCall) noexcept(false);

  static inline void Routine(AbstractModule *module) { 
    while (true) {
      module->ExecuteTask();
    }
  }

  // pure virtual functions
public:
  // Module initialization consists of few steps:
  // 1) Objects creation
  // 2) Main thread creation
  // 3) Probably additional threads creation
  // 4) Fills slots table (i.e. slots signatures -> slot)
  // 5) Sets slots priorities
  // Returns error status.
  // Do not throw exceptions, returns code error instead.
  virtual ModuleInitErrorStatus Init() noexcept = 0;

  // fields 
private:
  api::String id_; // module identifier; used to identify module; must be unique
  // contains slots signatures and binded handlers; signatures must be unique;
  api::HashMap<api::String, api::ScopedSlotWrapper> slots_;
  // Associated with this module queue of tasks. Kernel will push tasks to this
  // queue.
  api::UnboundedPriorityBlockingQueue<api::TaskWrapper> tasks_queue_;
};
} // namespace impl
#endif // !APPLICATION_IMPLDETAILS_ABSTRACTMODULE_HPP_
