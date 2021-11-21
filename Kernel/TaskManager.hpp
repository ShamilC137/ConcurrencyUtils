#ifndef APPLICATION_KERNEL_TASK_MANAGER_HPP_
#define APPLICATION_KERNEL_TASK_MANAGER_HPP_

// current project
#include "../API/DataStructures/Containers/HashMap.hpp"
#include "../API/DataStructures/Containers/Vector.hpp"
#include "../API/DataStructures/Multithreading/UnboundedPriorityBlockingQueue.hpp"
#include "../API/DataStructures/TaskWrapper.hpp"
#include "../ImplDetails/AbstractModule.hpp"
#include "KernelUtilities.hpp"

namespace api {
template <class T, class U>
class Pair {
 public:
  T first;
  U second;
};
}  // namespace api

namespace impl {
// FIXME: stub. Returns next signal + container of connected slots with its
// priorities
inline api::Pair<api::String, api::Vector<api::Pair<api::String, int>>>
GetNextEntryStub() {
  return {};
}
}  // namespace impl

namespace kernel {
struct ModuleNotFound : std::exception {
  using Base = std::exception;
  using Base::Base;
  using Base::what;
  using Base::operator=;
};

struct SlotNotFound : std::exception {
  using Base = std::exception;
  using Base::Base;
  using Base::what;
  using Base::operator=;
};

/*
 * TaskManagers control kernel task queue. Creates a connections table (signal
 * -> vector of slots) and uses this table to deliver the task. All slots
 * are checked for availabitility, i.e. correspond module provide this slots.
 * Task sending steps:
 * 1) set the number of task acceptors (for whole task)
 * 2) set the target slot signature for all task wrappers
 */
class TaskManager {
 public:
  TaskManager();

  // Adds module to module table.
  void AddModule(ModuleDescriptor *md);

  // Removes module from module table.
  void DeleteModule(const ModuleDescriptor *md);

  // Adds task to general queue. Blocks caller until task is pushed
  void PushTask(const api::TaskWrapper &task);

  // Fills connections table (signal -> vector of slots). Uses
  // configuration file for searching signals and slots. Uses modules
  // for slots existence check.
  // throws: ModuleNotFound if module signature is not present;
  //         SlotNotFound if specified module slot is not present.
  void FillConnectionsTable() noexcept(false);

  // Extracts the next task from queue and sends it to correspong module.
  // Throws: out_of_range if signal is not present
  bool SendNextTask() noexcept(false);

 private:
  // Searches for module descriptor.
  ModuleDescriptor *FindDescriptor(const api::String &id);

 private:
  // The kernel will take tasks from this queue. Modules will push tasks to the
  // queue with "Emit" function.
  api::UnboundedPriorityBlockingQueue<api::TaskWrapper> tasks_queue_;
  // Containter of connections; connection model:
  // signal -> container of slots (contains pair: slot signature and associated
  // module).
  // Signals signatures are extracted from project configuration file.
  // Cannot work with objects for now.
  api::HashMap<api::String,
               api::Vector<api::Pair<api::String, ModuleDescriptor *>>>
      connections_signatures_;
  // Container of modules.
  api::Vector<ModuleDescriptor *> modules_;
};
}  // namespace kernel
#endif  // APPLICATION_KERNEL_TASK_MANAGER_HPP_
