#ifndef APPLICATION_IMPLDETAILS_TASKDETAILS_HPP_
#define APPLICATION_IMPLDETAILS_TASKDETAILS_HPP_

// current project
#include "../../Config.hpp"
#include "../API/DataStructures/Containers/String.hpp"
#include "../API/DataStructures/Multithreading/Atomics.hpp"
#include "Utility.hpp"

// conditional headers
#if ALIGNED_ALLOCATOR_USAGE
#include "../API/Memory/AlignedAllocator.hpp"
#endif

// STL
// Allocator construct and destroy are deprecated in C++17 so I we have to use
// allocator_traits because there are some checks that disallow to compile
// code with these members. Use of define will not fix problem because
// other header also may use "memory" header.
#include <memory> // for allocator_traits

namespace impl {
// Basic task which contains RTTI about derived classes and multithreading
// synchronization functions.
class BaseTask {
  // ctor and dtor
public:
  // signal_sig - identifier emitter
  // is_blocking_task - if true, task will be blocked in emitter until
  // all target threads complete their routines. Applyed for derived classes,
  // i.e. Task.
  // idseq - types identificators sequence which describes derived classes
  // retid - nullptr if task will have no return value and contain type
  // identifier of return type otherwise.
  BaseTask(const api::String &signal_sig, const bool is_blocking_task,
           const int *idseq, const int *retid) noexcept;

  // throws
  virtual ~BaseTask() noexcept(false);
  BaseTask &operator=(const BaseTask &rhs) = delete;
  BaseTask &operator=(BaseTask &&rhs) = delete;

  // getters
public:
  // Return size in bytes of current type object; used to deallocate object
  // captured with pointer to base class.
  [[nodiscard]] inline virtual std::size_t SizeInBytes() const noexcept {
    return sizeof(BaseTask);
  }
  // Returns creator module identifier
  [[nodiscard]] inline const api::String &GetCreatorModuleID() const noexcept {
    return signal_sig_;
  }

  // Returns sequence of derived task types identifiers.
  [[nodiscard]] inline const int *GetIDSequencePtr() const noexcept {
    return idseq_ptr_;
  }

  // Returns the task return type identifier
  [[nodiscard]] inline const int *GetRetIDPtr() const noexcept {
    return retid_ptr_;
  }

  // Return current number of references.
  [[nodiscard]] inline unsigned char GetNumOfRefs() const noexcept {
    return nreferences_.load(api::MemoryOrder::relaxed);
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
  // instances). Used to automatic deletion in TaskWrapper
  inline void SetNumOfRefs(const unsigned char nreferences) noexcept {
    nreferences_.store(nreferences, api::MemoryOrder::relaxed);
  }

  // Set the number of task acceptors (i.e. slots). When slot complete its work,
  // it decrements number of acceptors.
  // may throw in derived class
  virtual inline void
  SetNumOfAcceptors(const unsigned char nacceptors) noexcept(false) {
    waiters_semaphore_.store(
        nacceptors_.add(nacceptors, api::MemoryOrder::relaxed),
        api::MemoryOrder::relaxed);
  }

  // Decrements number of references and returns new value.
  unsigned char DecrementNumOfRefs() noexcept {
    return nreferences_.sub(1u, api::MemoryOrder::relaxed);
  }

  // Increments number of references and returns new value.
  unsigned char IncremenetNumOfRefs() noexcept {
    return nreferences_.add(1u, api::MemoryOrder::relaxed);
  }

  // Unblock at least one thread blocked in a waiting operation on its atomic
  // object. Decrements the counter of acceptors.
  void NotifyAboutComplete() noexcept;

  // sync operations
public:
  // Potentially blocks the calling thread until unblocked be a notifying
  // operation and load return value equal to expected.
  // Waiting until number of acceptors and expected value become equal.
  // By default, assumed that task emitter will be wait for task completion.
  // If few slots are binded to the task and the order of execution is important
  // this function can be used as a synchronization.
  // throws
  void Wait(const unsigned char expected_value = 0) noexcept(false);

private:
  api::String signal_sig_;             // Module identefier
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
