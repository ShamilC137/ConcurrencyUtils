#ifndef APPLICATION_IMPLDETAILS_TASKDETAILS_HPP_
#define APPLICATION_IMPLDETAILS_TASKDETAILS_HPP_

// current project
#include "../API/DataStructures/Containers/String.hpp"
#include "../API/DataStructures/Multithreading/Atomics.hpp"
#include "Utility.hpp"

namespace impl {
// Basic task which contains RTTI about derived classes and multithreading
// synchronization functions.
class BaseTask {
  // ctor and dtor
public:
  // module_id - identifier of creator module
  // is_blocking_task - if true, task will be blocked in emitter until
  // all target threads complete their routines. Applyed for derived classes,
  // i.e. Task.
  // idseq - types identificators sequence which describes derived classes
  // retid - nullptr if task will have no return value and contain type
  // identifier of return type otherwise.
  BaseTask(const api::String &module_id, const bool is_blocking_task,
           const int *idseq, const int *retid) noexcept;

  // throws
  virtual ~BaseTask() noexcept(false);

  // getters
public:
  // Returns creator module identifier
  [[nodiscard]] inline const api::String &GetCreatorModuleID() const noexcept {
    return mid_;
  }

  // Returns sequence of derived task types identifiers.
  [[nodiscard]] inline const int *GetIDSequencePtr() const noexcept {
    return idseq_ptr_;
  }

  // Returns the task return type identifier
  [[nodiscard]] inline const int *GetRetIDPtr() const noexcept {
    return retid_ptr_;
  }

  // Shows that task must return value (i.e. have type ReturnTask)
  [[nodiscard]] inline bool IsMustReturn() const noexcept {
    return static_cast<bool>(retid_ptr_); // explicit cast
  }

  // Returns true if task must wait for target slot to complete
  // Safe. Read only.
  [[nodiscard]] inline bool IsBlockingTask() const noexcept {
    return is_blocking_task_;
  }

  // Shows that target threads still working
  [[nodiscard]] inline bool IsWaitable() const noexcept {
    return nacceptors_.load(api::MemoryOrder::relaxed) >
           static_cast<unsigned char>(
               is_blocking_task_); // task itself is not considered as waitable
                                   // routine
  }

  // modifiers
public:
  // Sets the basic number of references to task (i.e. number of this task
  // instances). Used to automatic deletion in TaskWrapper.
  // Safe. Only kernel thread can set number of references
  inline void SetNumOfRefs(const unsigned char nreferences) noexcept {
    nreferences_.store(nreferences, api::MemoryOrder::relaxed);
  }

  // Set the number of task acceptors (i.e. slots). When slot complete its work,
  // it decrements number of acceptors.
  // Safe. Only kernel thread can set number of acceptors
  inline void SetNumOfAcceptors(const unsigned char nacceptors) noexcept {
    waiters_semaphore_.store(
        nacceptors_.add(nacceptors, api::MemoryOrder::relaxed),
        api::MemoryOrder::relaxed);
  }

  // Decrements number of references and returns new value.
  // Safe. Atomic counter
  unsigned char DecrementNumOfRefs() noexcept {
    return nreferences_.sub(1, api::MemoryOrder::relaxed);
  }

  // Unblock at least one thread blocked in a waiting operation on its atomic
  // object. Decrements the counter of acceptors.
  void NotifyAboutComplete();

  // sync operations
public:
  // Potentially blocks the calling thread until unblocked be a notifying
  // operation and load return value equal to expected.
  // Waiting until number of acceptors and expected value become equal.
  // By default, assumed that task emitter will be wait for task completion.
  // If few slots are binded to the task and the order of execution is important
  // this function can be used as a synchronization.
  void Wait(const unsigned char expected_value = 0) noexcept(false);

private:
  api::String mid_;             // Module identefierm
  const bool is_blocking_task_; // If true, derived task will be blocked until
                                // target threads routine completion.
  const int *idseq_ptr_;        // Derived classes types identifiers sequence.
  const int *retid_ptr_;        // Derived classes return type identifier.
  api::Atomic<unsigned char> nreferences_; // Number of references to this task.
                                           // Settted by kernel.
  api::Atomic<unsigned char> nacceptors_;  // Number of this task acceptors.
                                           // Setted by kernel.
  api::Atomic<unsigned char> waiters_semaphore_; // Number of current waiters.
};
} // namespace impl

#endif // !APPLICATION_IMPLDETAILS_TASKDETAILS_HPP_
