#ifndef APPLICATION_IMPLDETAILS_SLOTDETAILS_HPP_
#define APPLICATION_IMPLDETAILS_SLOTDETAILS_HPP_
// current project
#include "../API/DataStructures/Containers/HashMap.hpp"
#include "../API/DataStructures/TaskWrapper.hpp"
#include "TaskDetails.hpp"
#include "Utility.hpp"

namespace impl {
/// <summary>
///   Basic slot which contains RTTI about derived classes, its priority (if
///   given), and operator () that calls correct underlying function with given
///   task.
/// </summary>
class BaseSlot {
 public:
  /// <summary>
  ///   If return type is void, retid becomes nullptr
  /// </summary>
  /// <param name="idseq">
  ///   derived class types identifiers sequence
  /// </param>
  /// <param name="retid">
  ///   derived class return type identifier
  /// </param>
  /// <returns></returns>
  /// <multithreading>
  ///   safe
  /// </multithreading>
  BaseSlot(const int *idseq, const int *retid) noexcept;

  inline virtual ~BaseSlot() noexcept {
    assert(!is_executed_.test(api::MemoryOrder::relaxed) &&
           "Slot currently executed");
  }

  BaseSlot &operator=(const BaseSlot &rhs) = delete;
  BaseSlot &operator=(BaseSlot &&rhs) = delete;

  /// <summary>
  ///   Returns derived class types identifiers sequence
  /// </summary>
  /// <returns>
  ///   returns pointer to types ids sequence
  /// </returns>
  /// <multithreading>
  ///   safe
  /// </multithreading>
  [[nodiscard]] inline const int *GetIDSequencePtr() const noexcept {
    return idseq_ptr_;
  }

  /// <summary>
  ///   Return derived class return type identifier
  /// </summary>
  /// <returns>
  ///   Return nullptr is derived class is Task and return type
  ///   identifier if derived class is ReturnTask
  /// </returns>
  /// <multithreading>
  ///   safe
  /// </multithreading>
  [[nodiscard]] inline const int *GetRetIDPtr() const noexcept {
    return retid_ptr_;
  }

  /// <summary>
  ///   Sets execution flag and returns its prior state.
  /// </summary>
  /// <param name="order">
  ///   test_and_set memory order
  /// </param>
  /// <returns>
  ///   prior execution flag state
  /// </returns>
  /// <multithreading>
  ///   safe
  /// </multithreading>
  inline bool Execute(
      api::MemoryOrder order = api::MemoryOrder::relaxed) noexcept {
    return is_executed_.test_and_set(order);  // exchange(prev, true)
  }

  /// <summary>
  ///   Unsets execution flag
  /// </summary>
  /// <param name="order">
  ///   clear memory order
  /// </param>
  /// <multithreading>
  ///   safe
  /// </multithreading>
  inline void Release(
      api::MemoryOrder order = api::MemoryOrder::relaxed) noexcept {
    is_executed_.clear(order);
  }

  // Sets the slot priority

  /// <summary>
  ///   Sets associated with this signal slot priority
  /// </summary>
  /// <param name="signal">
  ///   signal signature
  /// </param>
  /// <param name="priority">
  ///   slot priority
  /// </param>
  /// <multithreading>
  ///   unsafe
  /// </multithreading>
  void SetPriority(const api::String &signal, const int priority) noexcept {
    assert(priority != 0);
    priorities_[signal] = priority;
  }

  /// <summary>
  ///   Return current priority
  /// </summary>
  /// <param name="signal"></param>
  /// <returns>
  ///   associated with this signal priority
  /// </returns>
  /// <exception type="std::out_of_range">
  ///   Thrown if priority with given signal does not exist
  /// </exception>
  /// <multithreading>
  ///   unsafe
  /// </multithreading>
  [[nodiscard]] inline int GetPriority(const api::String &signal) const
      noexcept(false) {
    return priorities_.at(signal);
  }

  /// <summary>
  ///   Calls underlying function. May throw underlying function exception.
  /// </summary>
  /// <param name="task">
  ///   Task to execute
  /// </param
  /// <exception type="api::BadSlotCall")
  ///   Thrown when task and slot parameters are incompatible
  /// </exception>
  /// <exception type="api::Deadlock">
  ///   Thrown if deadlock situation is detected
  /// </exception>
  /// <multithreading>
  ///   undefined
  /// </multithreading>
  void operator()(api::TaskWrapper &task) noexcept(false);

  /// <summary>
  ///   Calls underlying function. May throw underlying function exception.
  /// </summary>
  /// <param name="task">
  ///   Task to execute
  /// </param
  /// <exception type="api::BadSlotCall")
  ///   Thrown when task and slot parameters are incompatible
  /// </exception>
  /// <exception type="api::Deadlock">
  ///   Thrown if deadlock situation is detected
  /// </exception>
  /// <multithreading>
  ///   undefined
  /// </multithreading>
  void operator()(api::TaskWrapper &&task) noexcept(false);

 protected:
  // potentially throws: api::Deadlock, api::BadSlotCall
  // Makes real call to underlying function
  virtual void RealCall(api::TaskWrapper &task) noexcept(false) = 0;

 private:
  const int *idseq_ptr_;  // derived class types identifiers sequence
  const int *retid_ptr_;  // derived class return type identifier
  // Number of current exectures of this slot. All users of slots must change
  // number of executers themselves.
  // api::Atomic<unsigned char> nexecuters_;
  api::AtomicFlag is_executed_;
  // slot priority; slot with the highest prioprity
  // called first; if no priority is set, -1 is used.
  // Its priotiry depends from signal. Kernel sets the priorities
  api::HashMap<api::String, int> priorities_;
};
}  // namespace impl

#endif  // !APPLICATION_IMPLDETAILS_SLOTDETAILS_HPP_
