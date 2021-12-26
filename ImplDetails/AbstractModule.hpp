#ifndef APPLICATION_IMPLDETAILS_ABSTRACTMODULE_HPP_
#define APPLICATION_IMPLDETAILS_ABSTRACTMODULE_HPP_

// current project
#include "../API/DataStructures/Containers/HashMap.hpp"
#include "../API/DataStructures/Containers/String.hpp"
#include "../API/DataStructures/Containers/Vector.hpp"
#include "../API/DataStructures/Multithreading/Atomics.hpp"
#include "../API/DataStructures/Multithreading/SharedLockGuard.hpp"
#include "../API/DataStructures/Multithreading/SharedMutex.hpp"
#include "../API/DataStructures/Multithreading/UnboundedPriorityBlockingQueue.hpp"
#include "../API/DataStructures/Slot.hpp"
#include "../API/DataStructures/Task.hpp"
#include "../API/DataStructures/TaskWrapper.hpp"
#include "../Config.hpp"
#include "ImplAPI/Errors.hpp"

// STD
#include <cassert>
#include <memory>

#if TEST_ENABLED
namespace test {
class TestModule;
}
#endif

namespace api {
// indicates that slot call must be done whether it already called or not
struct ForceSlotCall {};
}  // namespace api

namespace impl {
/// <summary>
///   AbstractModule is the basic module for other program modules. Program
///   module is used to extend application functionality. AbstactModule contains
///   all features that all modules must contain. AbstactModule provide
///   functions for working with tasks queue and slots
/// </summary>
class AbstractModule {
  // Ctors
 public:
  /// <param name="mid"> module identifier </param>
  AbstractModule(const api::String &mid);

  virtual ~AbstractModule() noexcept;

  // getters
 public:
  /// <summary>
  ///   Returns module id
  /// </summary>
  /// <returns> module id </returns>
  /// <multithreading> safe </multithreading>
  [[nodiscard]] inline const api::String &GetId() const noexcept { return id_; }

  /// <summary>
  ///   Extracts all slots signatures.
  /// </summary>
  /// <returns> vector of signatures </returns>
  /// <multithreading> safe </multithreading>
  [[nodiscard]] api::Vector<api::String> GetSlotsSignatures() const;

  // modifying functions
 public:
  /// <summary>
  ///   Adds new slot to the slots' table.
  ///   Slot must be created on heap. Module takes ownership on the slot.
  /// </summary>
  /// <param name="slot_sig"> slot signature </param>
  /// <param name="slot"> pointer to slot </param>
  /// <returns>
  ///   True if slot is added and false otherwise. If slot is not added, it's
  ///   deleted.
  /// </returns>
  /// <multithreading> safe </multithreading>
  bool AddSlotToTable(const api::String &slot_sig, impl::BaseSlot *slot);

  /// <returns> tasks' queue size </returns>
  /// <multithreading> safe </multithreading>
  [[nodiscard]] inline std::size_t TaskQueueSize() const noexcept {
    return tasks_queue_.Size();
  }

  /// <summary>
  ///   Extracts a new task from the tasks queue. Potentially block until
  ///   a new task appears or queue is modified.
  /// </summary>
  /// <returns> extracted task </returns>
  /// <multithreading> safe </multithreading>
  /// <exception type="api::PopFailed">
  ///   Thrown when tasks queue is empty
  /// </exception>
  /// <multithreading> safe </multithreading>
  [[nodiscard]] inline api::TaskWrapper ExtractTask() noexcept(false) {
    return tasks_queue_.Pop();
  }

  /// <summary>
  ///   Extracts a new task from the tasks queue. .Do not block caller thread if
  ///   queue is empty or if queue busy.
  /// </summary>
  /// <returns>
  ///   Extracted task
  /// </returns>
  /// <exception type="api::PopFailed">
  ///   Thrown if tasks queue is empty
  /// </exception>
  /// <multithreading> safe </multithreading>
  [[nodiscard]] api::TaskWrapper TryExtractTask() noexcept(false);

  /// <summary>
  ///   Pushes a new task to the tasks queue. Potentially blocks if queue is
  ///   modified.
  /// </summary>
  /// <param name="task"> task </param>
  /// <multithreading> safe </multithreading>
  inline void PushTask(api::TaskWrapper task) { tasks_queue_.Push(task); }

  /// <summary>
  ///   Pushes a new task to the task queue. Not blocks caller thread.
  /// </summary>
  /// <param name="task"> task </param>
  /// <returns>
  ///   Returns true if push operation is completed, otherwise false
  /// </returns>
  /// <multithreading> safe </multithreading>
  inline bool TryPushTask(api::TaskWrapper task) {
    return tasks_queue_.TryPush(task);
  }

  /// <summary>
  ///   Extract task from tasks queue and execute it. Potentially blocks caller
  ///   thread if queue is modified or empty or
  ///   slot have priority.Only one call to underlying slot is allowed.
  /// </summary>
  /// <returns>
  ///   kOk if call was compeleted successfully
  ///   kBusy if slot already called
  /// </returns>
  /// <exception type="std::out_of_range">
  ///   Thrown if slot with task signature does not exist
  /// </exception>
  /// <exception type="api::BadSlotCall">
  ///   Thrown if slot and task parameters types are incompatible
  /// </exception>
  /// <exception type="api::PopFailed">
  ///   Thrown if task queue is empty
  /// </exception>
  /// <multithreading>
  ///   safe
  /// </multithreading>
  ThreadResourceErrorStatus ExecuteNextTask() noexcept(false);

  /// <summary>
  ///   Extract task from tasks queue and execute it. Potentially blocks caller
  ///   thread if queue is modified or empty or slot have priority.Makes call
  ///   whether slot busy or not.
  /// </summary>
  /// <param name=""> force call tag</param>
  /// <exception type="std::out_of_range">
  ///   Thrown if slot with task signature does not exist
  /// </exception>
  /// <exception type="api::BadSlotCall">
  ///   Thrown if slot and task parameters types are incompatible
  /// </exception>
  /// <exception type="api::PopFailed">
  ///   Thrown if task queue is empty
  /// </exception>
  /// <multithreading>
  ///   safe
  /// </multithreading>
  void ExecuteNextTask(api::ForceSlotCall) noexcept(false);

  /// <summary>
  ///   Takes task and execute it. Potentially blocks caller thread if slot have
  ///   priority.
  /// </summary>
  /// <param name="task"> task </param>
  /// <returns>
  ///   kOk if called was completed successfully
  ///   kBusy if slot already called
  /// </returns>
  /// <exception type="std::out_of_range">
  ///   Thrown if task target slot does not exist
  /// </exception>
  /// <exception type="api::BadSlotCall">
  ///   Thrown if slot and task types are incompatible
  /// </exception>
  /// <multithreading>
  ///   safe
  /// </multithreading>
  ThreadResourceErrorStatus ExecuteTask(api::TaskWrapper task) const
      noexcept(false);

  /// <summary>
  ///   Extract task from tasks queue and execute it. Potentially blocks caller
  ///   thread if queue is modified or empty or slot have priority.Makes call
  ///   whether slot busy or not.
  /// </summary>
  /// <param name="task"> task </param>
  /// <param name=""> force call tag </param>
  /// <exception type="std::out_of_range">
  ///   Thrown if task target slot does not exist
  /// </exception>
  /// <exception type="api::BadSlotCall">
  ///   Thrown if slot and task types are incompatible
  /// </exception>
  /// <multithreading>
  ///   safe
  /// </multithreading>
  void ExecuteTask(api::TaskWrapper task, api::ForceSlotCall) const
      noexcept(false);

  /// <summary>
  ///   Reads the configuration file and sets added to the slots' table slots
  ///   priorities.
  /// </summary>
  /// <exception std::out_of_range>
  ///   Thrown if read slot signature is not added to the slots' table
  /// </exception>
  void SetSlotsPriorities() noexcept(false);

  // pure virtual functions
 public:
  /// <summary>
  ///   Module initialization consists of few steps:
  ///   1) Objects creation
  ///   2) Threads creation
  ///   3) Fills slots table (i.e. slots signatures -> slot)
  ///   4) Sets slots priorities
  ///   Does not throw exceptions, returns code error instead
  /// </summary>
  /// <returns> initialization error status </returns>
  /// <multithreading> totally unsafe </multithreading>
  virtual ModuleInitErrorStatus Init() noexcept = 0;

  // fields
 private:
  api::String
      id_;  // module identifier; used to identify module; must be unique
  // contains slots signatures and binded handlers; signatures must be unique;
  api::HashMap<api::String, std::unique_ptr<BaseSlot>> slots_;
  // Associated with this module queue of tasks. Kernel will push tasks to this
  // queue.
  api::UnboundedPriorityBlockingQueue<api::TaskWrapper> tasks_queue_;

  mutable api::SharedMutex slots_mutex_;

#if TEST_ENABLED
  friend class test::TestModule;
#endif
};
}  // namespace impl
#endif  // !APPLICATION_IMPLDETAILS_ABSTRACTMODULE_HPP_
