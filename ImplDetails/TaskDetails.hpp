#ifndef APPLICATION_IMPLDETAILS_TASKDETAILS_HPP_
#define APPLICATION_IMPLDETAILS_TASKDETAILS_HPP_

// current project
#include "../../Config.hpp"
#include "../API/DataStructures/Containers/String.hpp"
#include "../API/DataStructures/Multithreading/Atomics.hpp"
#include "Utility.hpp"

// STL
// Allocator construct and destroy are deprecated in C++17 so I we have to use
// allocator_traits because there are some checks that disallow to compile
// code with these members. Use of define will not fix problem because
// other header also may use "memory" header.
#include <cassert>
#include <memory> // for allocator_traits

namespace api {
// Used to compare to task.
enum class TaskPriority : unsigned char { kLowPriority, kHighPriotity };
} // namespace api

namespace impl {
// Basic task which contains RTTI about derived classes and multithreading
// synchronization functions.
class BaseTask {
  // ctor and dtor
public:
  // signal_sig - identifier emitter
  // all target threads complete their routines. Applyed for derived classes,
  // i.e. Task.
  // idseq - types identificators sequence which describes derived classes
  // retid - nullptr if task will have no return value and contain type
  // identifier of return type otherwise.
  BaseTask(const api::String &signal_sig, api::TaskPriority priority,
           const int *idseq, const int *retid) noexcept;

  virtual ~BaseTask() noexcept;
  BaseTask &operator=(const BaseTask &rhs) = delete;
  BaseTask &operator=(BaseTask &&rhs) = delete;

  // getters
public:
  // Return size in bytes of current type object; used to deallocate object
  // captured with pointer to base class.
  [[nodiscard]] inline virtual std::size_t SizeInBytes() const noexcept {
    return sizeof(BaseTask);
  }

  // Returns associated with this task signal signature
  [[nodiscard]] inline const api::String &GetCausedSignal() const noexcept {
    return caused_signal_sig_;
  }

  // Returns sequence of derived task types identifiers.
  [[nodiscard]] inline const int *GetIDSequencePtr() const noexcept {
    return idseq_ptr_;
  }

  // Returns the task return type identifier
  [[nodiscard]] inline const int *GetRetIDPtr() const noexcept {
    return retid_ptr_;
  }

  // Return current number of references with load().
  [[nodiscard]] inline unsigned char GetNumOfRefs(
      api::MemoryOrder order = api::MemoryOrder::acquire) const noexcept {
    return nreferences_.load(order);
  }

  // Shows that task must return value (i.e. have type ReturnTask)
  [[nodiscard]] inline bool IsMustReturn() const noexcept {
    return static_cast<bool>(retid_ptr_); // explicit cast
  }

  // Return task's priority
  [[nodiscard]] inline api::TaskPriority GetPriority() const noexcept {
    return priority_;
  }

  // Shows that target threads still working (with load())
  [[nodiscard]] inline bool IsWaitable(
      api::MemoryOrder order = api::MemoryOrder::acquire) const noexcept {
    return nacceptors_.load(order); // task itself is not considered as
                                    // waitable routine
  }

  // modifiers
public:
  // Sets the basic number of references to task (i.e. number of this task
  // instances, with store()). Used to automatic deletion in TaskWrapper.
  inline void
  SetNumOfRefs(const unsigned char nreferences,
               api::MemoryOrder order = api::MemoryOrder::release) noexcept {
    nreferences_.store(nreferences, order);
  }

  // Set the number of task acceptors (i.e. slots, with store()). When slot
  // complete its work, it implicitely decrements number of acceptors.
  // May throw in derived class
  virtual inline void SetNumOfAcceptors(
      const unsigned char nacceptors,
      api::MemoryOrder order = api::MemoryOrder::release) noexcept {
    nacceptors_.store(nacceptors, order);
  }

  // Decrements number of references and returns new value.
  unsigned char DecrementNumOfRefs(
      api::MemoryOrder order = api::MemoryOrder::acq_rel) noexcept {
    return nreferences_.sub(1u, order);
  }

  // Increments number of references and returns new value.
  unsigned char IncrementNumOfRefs(
      api::MemoryOrder order = api::MemoryOrder::acq_rel) noexcept {
    return nreferences_.add(1u, order);
  }

  // Unblock at least one thread blocked in a waiting operation on its atomic
  // object. Decrements the counter of acceptors. Automaticly deletes arguments
  // if task is not waitable anymore
  void NotifyAboutComplete() noexcept;

  // Deletes task arguments
  virtual void ClearArguments() = 0;

  // sync operations
public:
  // Potentially blocks the calling thread until unblocked be a notifying
  // operation and load return value equal to expected.
  // Waiting until number of acceptors (i.e. uncompleted slots)
  // and expected value become equal.
  // By default, assumed that task emitter will be wait for task completion.
  // If few slots are binded to the task and the order of execution is important
  // this function can be used as a synchronization.
  // Task itself always have expected_value = 0.
  // throws: api::Deadlock
  void Wait(const unsigned char expected_value = 0) noexcept(false);

private:
  api::String caused_signal_sig_;    // Module identefier
  const api::TaskPriority priority_; // Task priority is used to compare tasks.
  const int *idseq_ptr_; // Derived classes types identifiers sequence.
  const int *retid_ptr_; // Derived classes return type identifier.
  api::Atomic<unsigned char> nreferences_; // Number of references to this task.
  api::Atomic<unsigned char>
      nacceptors_; // Number of this task acceptors.
                   // Setted by kernel. Reflects number of slots which will be
                   // execute this task
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
} // namespace impl
#endif // !APPLICATION_IMPLDETAILS_TASKDETAILS_HPP_
