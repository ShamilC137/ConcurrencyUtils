#ifndef APPLICATION_API_MULTITHREADING_UNBOUNDEDPRIORITYBLOCKINGQUEUE_HPP_
#define APPLICATION_API_MULTITHREADING_UNBOUNDEDPRIORITYBLOCKINGQUEUE_HPP_

// current project
#include "Atomics.hpp"
#include "ConditionVariable.hpp"
#include "Mutex.hpp"
#include "ScopedLock.hpp"
#include "UniqueLock.hpp"

#include "../Containers/Deque.hpp"

// STL
#include <exception>

namespace api {
struct PopFailed : std::exception {
  using Base = std::exception;
  using Base::Base;
  using Base::what;
};
// This container represents unbounded priority queue allowing one writter and
// multiple readers. "Push" and "Emplace" operations just adding a new element
// to the container, insert position is dependent from priority. By default, new
// element is inserted at the end of container and then moved to the right
// position.

// Modifying operations block other modifying operations and new read
// operations. Modifying operations are not performed until current read
// operations are completed. Only one modifying operation and many read
// operations can be performed at the same time. All read operations wait
// for the modify operation completion.

// WARNING: careless usage of this container methods to implement other its
// methods may cause deadlock!
template <class Value, class Container = api::Deque<Value>>
class UnboundedPriorityBlockingQueue {
public:
  // STL compatible type aliases
  using value_type = typename Container::value_type;
  using reference = typename Container::reference;
  using const_reference = typename Container::const_reference;
  using size_type = typename Container::size_type;
  using container_type = Container;

  static_assert(std::is_same_v<Value, value_type>,
                "held on type and adapted container type must be the same");

  // ctors
public:
  UnboundedPriorityBlockingQueue() = default;
  // copy and move operators are deleted cause of multithreading (must lock
  // write operations for correct copy)
  UnboundedPriorityBlockingQueue(const UnboundedPriorityBlockingQueue &) =
      delete;
  UnboundedPriorityBlockingQueue &
  operator=(const UnboundedPriorityBlockingQueue &) = delete;

private:
  // Waiting for the completion of desired modify operation. Potentially
  // block caller thread.
  void WaitForModifyOperation() const {
    while (want_to_modify_flag_.wait(true, MemoryOrder::relaxed))
      ;
  }

  // access operations
public:
  // Returns first element in queue. Potentially block if container is modified.
  [[nodiscard]] reference Front() noexcept(noexcept(container_.front())) {
    WaitForModifyOperation();
    read_semaphore_.fetch_add(1u, MemoryOrder::acquire);
    decltype(auto) result(container_.front());
    read_semaphore_.fetch_sub(1u, MemoryOrder::release);
    read_semaphore_.notify_one(); // notifies write operation
    return result;
  }

  // Returns first element in queue. Potentially block if container is modified.
  [[nodiscard]] const_reference Front() const
      noexcept(noexcept(container_.front())) {
    WaitForModifyOperation();
    read_semaphore_.fetch_add(1u, MemoryOrder::acquire);
    decltype(auto) result(container_.front());
    read_semaphore_.fetch_sub(1u, MemoryOrder::release);
    read_semaphore_.notify_one();
    return result;
  }

  // Returns last element in queue. Potentially block if container is modified.
  [[nodiscard]] reference Back() noexcept(noexcept(container_.back())) {
    WaitForModifyOperation();
    read_semaphore_.fetch_add(1u, MemoryOrder::acquire);
    decltype(auto) result(container_.back());
    read_semaphore_.fetch_sub(1u, MemoryOrder::release);
    read_semaphore_.notify_one();
    return result;
  }

  // Returns last element in queue. Potentially block if container is modified.
  [[nodiscard]] const_reference Back() const
      noexcept(noexcept(container_.back())) {
    WaitForModifyOperation();
    read_semaphore_.fetch_add(1u, MemoryOrder::acquire);
    decltype(auto) result(container_.back());
    read_semaphore_.fetch_sub(1u, MemoryOrder::release);
    read_semaphore_.notify_one();
    return result;
  }

  // Returns container state. Potentially blocks if container is modified.
  [[nodiscard]] bool Empty() const noexcept(noexcept(container_.empty())) {
    WaitForModifyOperation();
    read_semaphore_.fetch_add(1u, MemoryOrder::acquire);
    decltype(auto) result(container_.empty());
    read_semaphore_.fetch_sub(1u, MemoryOrder::release);
    read_semaphore_.notify_one();
    return result;
  }

  // Returns container state. Potentially blocks if container is modified.
  [[nodiscard]] size_type Size() const noexcept(noexcept(container_.size())) {
    WaitForModifyOperation();
    read_semaphore_.fetch_add(1, MemoryOrder::acquire);
    decltype(auto) result(container_.size());
    read_semaphore_.fetch_sub(1, MemoryOrder::release);
    read_semaphore_.notify_one();
    return result;
  }

private:
  // corrects last insertion with its priority
  void CorrectInsertion() {
    auto inserted{container_.rbegin()};
    auto swapable{++container_.rbegin()};
    const auto rend{container_.rend()};
    while (swapable != rend && *inserted > *swapable) {
      std::swap(*inserted++, *swapable++);
    }
  }

  // waiting for the completion of all exectuted read operation. Potentially
  // block caller thread.
  void WaitForReadOperations() const {
    // waiting until read_semaphore value will be 0
    for (auto old{read_semaphore_.load(MemoryOrder::relaxed)}; old != 0u;
         old = read_semaphore_.load(MemoryOrder::relaxed)) {
      read_semaphore_.wait(old, MemoryOrder::relaxed);
    }
  }

  // modify operations
public:
  // Adds new element to container. Potentially block if container already
  // modified.
  void Push(const_reference value) { Emplace(value); }

  // Adds new element to container. Potentially block if container already
  // modified.
  void Push(value_type &&value) { Emplace(std::move(value)); }

private:
  template <class... Args> void UnblockingEmplace(Args &&...args) {
    container_.emplace_back(std::forward<Args>(args)...);
    CorrectInsertion();
    want_to_modify_flag_.clear(api::MemoryOrder::release);
    want_to_modify_flag_.notify_all();
    rw_sync_cvar_.notify_all();
  }

public:
  // Adds new element to container. Potentially block if container already
  // modified.
  template <class... Args> void Emplace(Args &&...args) {
    ScopedLock<api::Mutex> lock(mutex_); // disable other modifications
    // there is no opportunity to return correct result of insert operations
    // because of elements reordering
    // disable new read operations
    want_to_modify_flag_.test_and_set(api::MemoryOrder::acquire);
    WaitForReadOperations();
    UnblockingEmplace(std::forward<Args>(args)...);
  }

private:
  value_type UnblockingPop() {
    auto last{std::move(container_.front())};
    container_.pop_front();
    want_to_modify_flag_.clear(MemoryOrder::release); // enable read ops
    want_to_modify_flag_.notify_all();
    return last;
  }

public:
  // Extract element from container and return this element. Potentially blocks
  // if container already modified or queue is empty.
  value_type Pop() {
    UniqueLock<Mutex> lock(mutex_); // disable other modifications
    while (container_.empty()) {
      rw_sync_cvar_.wait(lock);
    }
    // disable new read operations
    want_to_modify_flag_.test_and_set(MemoryOrder::acquire);
    WaitForReadOperations();
    return UnblockingPop();
  }

  // Add new element to container. If add operation cannot be performed,
  // returns false.
  // Not blocks caller thread.
  bool TryPush(const_reference value) { return TryEmplace(value); }

  // Add new element to container. If add operation cannot be performed,
  // returns false.
  // Not blocks caller thread.
  bool TryPush(value_type &&value) { return TryEmplace(std::move(value)); }

  // Add new element to container. If add operation cannot be performed, 
  // returns false.
  // Not blocks caller thread.
  template <class... Args> bool TryEmplace(Args &&...args) {
    api::UniqueLock<api::Mutex> lock(mutex_, boost::try_to_lock_t{});
    // prevent new read operations
    want_to_modify_flag_.test_and_set(api::MemoryOrder::acquire);
    if (!lock.owns_lock() ||
        read_semaphore_.load(api::MemoryOrder::relaxed) != 0u) {
      want_to_modify_flag_.clear(api::MemoryOrder::release);
      want_to_modify_flag_.notify_all();
      return false;
    }
    UnblockingEmplace(std::forward<Args>(args)...);
    return true;
  }

  // Extract last element of queue. If pop operation cannot be performed, 
  //  
  // exception PopFailed. Not blocks caller thread.
  value_type TryPop() noexcept(false) {
    api::UniqueLock<api::Mutex> lock(mutex_, boost::try_to_lock_t{});
    // prevent new read operations
    want_to_modify_flag_.test_and_set(api::MemoryOrder::acquire);
    if (!lock.owns_lock() ||
        read_semaphore_.load(api::MemoryOrder::relaxed) != 0u ||
        container_.empty()) {
      want_to_modify_flag_.clear(api::MemoryOrder::release);
      want_to_modify_flag_.notify_all();
      throw PopFailed("Failed to extract element");
    }
    return UnblockingPop();
  }

  // fields
private:
  // mutex for modifying operations sync
  Mutex mutex_;
  // prevents write operation while reading
  // if > 0 then write operations is disabled and allowed otherwise.
  mutable Atomic<unsigned int> read_semaphore_;
  // indicates that some thread want to make modify operation and disallows
  // new read operations
  mutable AtomicFlag want_to_modify_flag_;
  // conditional variable for modifying operations sync
  ConditionVariable rw_sync_cvar_;
  // queue
  Container container_; // All writes operations guarded by mutex
};
} // namespace api

#endif // !APPLICATION_API_MULTITHREADING_UNBOUNDEDPRIORITYBLOCKINGQUEUE_HPP_
