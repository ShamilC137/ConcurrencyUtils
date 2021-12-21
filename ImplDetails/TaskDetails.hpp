#ifndef APPLICATION_IMPLDETAILS_TASKDETAILS_HPP_
#define APPLICATION_IMPLDETAILS_TASKDETAILS_HPP_

// current project
#include "../../Config.hpp"
#include "../API/DataStructures/Containers/String.hpp"
#include "../API/DataStructures/Multithreading/Atomics.hpp"
#include "../API/DataStructures/Multithreading/ScopedLock.hpp"
#include "../API/DataStructures/Multithreading/SharedLockGuard.hpp"
#include "../API/DataStructures/Multithreading/SharedMutex.hpp"
#include "Utility.hpp"

// STL
#include <cassert>

namespace api {
// Used to compare to task.
enum class TaskPriority : unsigned char { kLowPriority, kHighPriotity };
}  // namespace api

namespace impl {
// Basic task which contains RTTI about derived classes and multithreading
// synchronization functions.
class BaseTask {
  // ctor and dtor
 public:
  /// <param name="signal_sig">
  ///   Emitter signal signature
  /// </param>
  /// <param name="priority">
  ///   Task order priority
  /// </param>
  /// <param name="idseq">
  ///   Sequence of parameters types identifiers
  /// </param>
  /// <param name="retid">
  ///   Nullptr if task have no return type (i.e. void) and return type
  ///   identifier otherwise
  /// </param>
  BaseTask(const api::String &signal_sig, api::TaskPriority priority,
           const int *idseq, const int *retid) noexcept;

  virtual ~BaseTask() noexcept;
  BaseTask &operator=(const BaseTask &rhs) = delete;
  BaseTask &operator=(BaseTask &&rhs) = delete;

  // getters
 public:
  /// <summary>
  ///   Sets caused signal. Used by kernel to remove module id from signal
  ///   signature
  /// </summary>
  /// <param name="signal">
  ///   Signature
  /// </param>
  /// <multithreading>
  ///   safe
  /// <multithreading>
  void SetCausedSignal(const api::String &signal);

  /// <summary>
  ///   Returns associated with this task signal signature
  /// </summary>
  /// <returns>
  ///   signal signature
  /// </returns>
  /// <multithreading>
  ///   safe
  /// <multithreading>
  [[nodiscard]] inline const api::String &GetCausedSignal() const noexcept;

  /// <summary>
  ///   Returns sequence of derived task types identifiers.
  /// </summary>
  /// <returns>
  ///   Sequence of parameters types identifiers
  /// </returns>
  /// <multithreading>
  ///   safe
  /// <multithreading>
  [[nodiscard]] inline const int *GetIDSequencePtr() const noexcept {
    return idseq_ptr_;
  }

  /// <returns>
  ///   Return type identifier (may be nullptr)
  /// </returns>
  /// <multithreading>
  ///   safe
  /// <multithreading>
  [[nodiscard]] inline const int *GetRetIDPtr() const noexcept {
    return retid_ptr_;
  }

  /// <summary>
  ///   Return current number of references with load().
  /// </summary>
  /// <param name="order">
  ///   load() memory order
  /// </param>
  /// <returns>
  ///   Current number of references (i.e. number of TaskWrapper associated
  ///   with this task)
  /// </returns>
  /// <multithreading>
  ///   safe
  /// </multithreading>
  [[nodiscard]] inline unsigned char GetNumOfRefs(
      api::MemoryOrder order = api::MemoryOrder::acquire) const noexcept {
    return nreferences_.load(order);
  }

  /// <summary>
  ///   Shows that task must return value (i.e. have return type)
  /// </summary>
  /// <returns>
  ///   True if task must return value and false otherwise.
  /// </returns>
  /// <multithreading>
  ///   safe
  /// </multithreading>
  [[nodiscard]] inline bool IsMustReturn() const noexcept {
    return static_cast<bool>(retid_ptr_);  // explicit cast
  }

  /// <returns>
  ///   Task priority
  /// </returns>
  [[nodiscard]] inline api::TaskPriority GetPriority() const noexcept {
    return priority_;
  }

  // Shows that target threads still working (with load())
  [[nodiscard]] inline bool IsWaitable(
      api::MemoryOrder order = api::MemoryOrder::acquire) const noexcept {
    return nacceptors_.load(order);  // task itself is not considered as
                                     // waitable routine
  }

  // modifiers
 public:
  /// <summary>
  ///   Sets the number of task acceptors (i.e. associated with this task
  ///   slots) with store(). When the slot completes its work, it automaticly
  ///   decrements number of acceptors. Used for acception priority.
  /// </summary>
  /// <param name="nacceptors">
  ///   Number of task acceptors
  /// </param>
  /// <param name="order">
  ///   store() operation memory order
  /// </param>
  /// <multithreading>
  ///   safe
  /// </multithreading>
  virtual inline void SetNumOfAcceptors(
      const unsigned char nacceptors,
      api::MemoryOrder order = api::MemoryOrder::release) noexcept {
    nacceptors_.store(nacceptors, order);
  }

  /// <summary>
  ///   Decrements number of references and returns new value.
  /// </summary>
  /// <param name="order">
  ///   sub() memory order
  /// </param>
  /// <returns>
  ///   New number of references
  /// </returns>
  unsigned char DecrementNumOfRefs(
      api::MemoryOrder order = api::MemoryOrder::relaxed) noexcept {
    return nreferences_.sub(1u, order);
  }

  /// <summary>
  ///   Increments number of references and returns new value.
  /// </summary>
  /// <param name="order">
  ///   add() memory order
  /// </param>
  /// <returns>
  ///   New number of references
  /// </returns>
  unsigned char IncrementNumOfRefs(
      api::MemoryOrder order = api::MemoryOrder::relaxed) noexcept {
    return nreferences_.add(1u, order);
  }

  /// <summary>
  ///   Notifies other threads about slot's work completion.
  ///   Unblock at least one thread blocked in a waiting operation on its atomic
  ///   object. Decrements the counter of acceptors. Automaticly deletes
  ///   arguments if task is not waitable anymore.
  /// </summary>
  void NotifyAboutComplete() noexcept;

  /// <summary>
  ///   Deletes task arguments
  /// </summary>
  virtual void ClearArguments() = 0;

  // sync operations
 public:
  // throws: api::Deadlock

  /// <summary>
  ///   Potentially blocks the calling thread until unblocked by the notifying
  ///   operation and current number of acceptors equals to expected.
  ///   Waiting until number of acceptors (i.e. uncompleted slots)and expected
  ///   value become equal.
  ///   If few slots are binded to the task and the order of execution is
  ///   important this function is used as a synchronization. Task itself
  ///   always have expected_value = 0.
  /// </summary>
  /// <param name="expected_value">
  ///   Expected number of remain acceptors
  /// </param>
  void Wait(const unsigned char expected_value = 0u) noexcept(false);

 private:
  api::String caused_signal_sig_;     // signal signature
  const api::TaskPriority priority_;  // Task priority is used to compare tasks.
  const int *idseq_ptr_;  // Derived classes types identifiers sequence.
  const int *retid_ptr_;  // Derived classes return type identifier.
  api::Atomic<unsigned char>
      nreferences_;  // Number of references to this task.
  api::Atomic<unsigned char>
      nacceptors_;  // Number of this task acceptors.
                    // Setted by kernel. Reflects number of slots which will be
                    // execute this task

  mutable api::SharedMutex signal_mutex_;
};

[[nodiscard]] inline bool operator==(const BaseTask &lhs, const BaseTask &rhs) {
  return lhs.GetPriority() == rhs.GetPriority();
}

[[nodiscard]] inline bool operator!=(const BaseTask &lhs, const BaseTask &rhs) {
  return !(lhs == rhs);
}

[[nodiscard]] inline bool operator<(const BaseTask &lhs, const BaseTask &rhs) {
  return lhs.GetPriority() < rhs.GetPriority();
}

[[nodiscard]] inline bool operator>(const BaseTask &lhs, const BaseTask &rhs) {
  return rhs < lhs;
}

[[nodiscard]] inline bool operator<=(const BaseTask &lhs, const BaseTask &rhs) {
  return !(lhs > rhs);
}

[[nodiscard]] inline bool operator>=(const BaseTask &lhs, const BaseTask &rhs) {
  return !(lhs < rhs);
}
}  // namespace impl
#endif  // !APPLICATION_IMPLDETAILS_TASKDETAILS_HPP_
