#ifndef APPLICATION_KERNEL_TASK_MANAGER_HPP_
#define APPLICATION_KERNEL_TASK_MANAGER_HPP_

// current project
#include "../API/DataStructures//Multithreading/SharedMutex.hpp"
#include "../API/DataStructures/Containers/HashMap.hpp"
#include "../API/DataStructures/Containers/Vector.hpp"
#include "../API/DataStructures/Multithreading/SharedLockGuard.hpp"
#include "../API/DataStructures/Multithreading/UnboundedPriorityBlockingQueue.hpp"
#include "../API/DataStructures/TaskWrapper.hpp"
#include "../ImplDetails/AbstractModule.hpp"
#include "KernelUtilities.hpp"

// FIXME: stub
namespace api {
template <class T, class U>
struct Pair {
  T first;
  U second;
};
}  // namespace api

namespace kernel {
struct ModuleNotFound : std::runtime_error {
  using Base = std::runtime_error;
  using Base::Base;
  using Base::what;
  using Base::operator=;
};

struct SlotNotFound : std::runtime_error {
  using Base = std::runtime_error;
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

  /// <summary>
  ///  module manipulation functions
  /// </summary>
 public:
  /// <summary>
  ///   Adds module to module table. Does not take ownership of the descriptor
  /// </summary>
  /// <param name="md"> pointer to module descriptor </param>
  /// <multithreading> safe </multithreading>
  void AddModule(ModuleDescriptor *md);

  /// <summary>
  ///   Removes module from module table
  /// </summary>
  /// <param name="md"> pointer to module descriptor </param>
  /// <multithreading> safe </multithreading>
  void EraseModule(const ModuleDescriptor *md);

  /// <summary>
  ///   task manipulation functions
  /// </summary>
 public:
  /// <summary>
  ///   Adds task to general queue. Blocks caller until task is pushed
  /// </summary>
  /// <param name="task"> task </param>
  /// <multithreading> safe </multithreading>
  void PushTask(const api::TaskWrapper &task);

  /// <summary>
  ///   Fills connections table (signal -> vector of slots). Uses
  ///   configuration file for searching signals and slots. Uses modules
  ///   for slots existence check.
  /// </summary>
  /// <exception type="ModuleNotFound">
  ///   Thrown when module signature is not present
  /// </exception>
  /// <exception type="SlotNotFound">
  ///   Thrown if specified module slot is not present
  /// </exception>
  /// <multithreading> safe </multithreading>
  void FillConnectionsTable() noexcept(false);

  /// <summary>
  ///   Extracts the next task from queue and sends it to correspong module. If
  ///   false is returned, task queue is empty.
  /// </summary>
  /// <exception type ="std::out_of_range">
  ///   Thrown if signal is not present
  /// </exception>
  /// <returns> true if task is sent and false otherwise </returns>
  /// <multithreading> partly safe:
  ///   if connection table is changed during this operation - UB
  /// </multithreading>
  bool SendNextTask() noexcept(false);

 private:
  /// <summary>
  ///   Searches for module descriptor
  /// </summary>
  /// <param name="id"> module identifier </param>
  /// <multithreading> safe </multithreading>
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
      connections_signatures_;  // guarded by shared mutex
  // Container of modules.
  api::Vector<ModuleDescriptor *> modules_;  // guarded by mutex

  api::SharedMutex modules_mutex_;
  api::SharedMutex connections_mutex_;
};
}  // namespace kernel
#endif  // APPLICATION_KERNEL_TASK_MANAGER_HPP_
