#ifndef APPLICATION_API_MULTITHREADING_UNBOUNDEDPRIORITYBLOCKINGQUEUE_HPP_
#define APPLICATION_API_MULTITHREADING_UNBOUNDEDPRIORITYBLOCKINGQUEUE_HPP_

// current project
#include "Atomics.hpp"
#include "ConditionVariable.hpp"
#include "Mutex.hpp"
#include "ScopedLock.hpp"
#include "UniqueLock.hpp"

#include "../Containers/Deque.hpp"

namespace api {
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
  // waiting for the completion of desired modify operation
  void WaitForModifyOperation() const {
    while (want_to_modify_flag_.test(MemoryOrder::relaxed)) {
      want_to_modify_flag_.wait(true, MemoryOrder::relaxed);
    }
  }

  // access operations
public:
  [[nodiscard]] reference Front() noexcept(noexcept(container_.front())) {
    WaitForModifyOperation();
    read_semaphore_.fetch_add(1u, MemoryOrder::acquire);
    decltype(auto) result(container_.front());
    read_semaphore_.fetch_sub(1u, MemoryOrder::release);
    read_semaphore_.notify_one(); // notifies write operation
    return result;
  }

  [[nodiscard]] const_reference Front() const
      noexcept(noexcept(container_.front())) {
    WaitForModifyOperation();
    read_semaphore_.fetch_add(1u, MemoryOrder::acquire);
    decltype(auto) result(container_.front());
    read_semaphore_.fetch_sub(1u, MemoryOrder::release);
    read_semaphore_.notify_one();
    return result;
  }

  [[nodiscard]] reference Back() noexcept(noexcept(container_.back())) {
    WaitForModifyOperation();
    read_semaphore_.fetch_add(1u, MemoryOrder::acquire);
    decltype(auto) result(container_.back());
    read_semaphore_.fetch_sub(1u, MemoryOrder::release);
    read_semaphore_.notify_one();
    return result;
  }

  [[nodiscard]] const_reference Back() const
      noexcept(noexcept(container_.back())) {
    WaitForModifyOperation();
    read_semaphore_.fetch_add(1u, MemoryOrder::acquire);
    decltype(auto) result(container_.back());
    read_semaphore_.fetch_sub(1u, MemoryOrder::release);
    read_semaphore_.notify_one();
    return result;
  }

  [[nodiscard]] bool Empty() const noexcept(noexcept(container_.empty())) {
    WaitForModifyOperation();
    read_semaphore_.fetch_add(1u, MemoryOrder::acquire);
    decltype(auto) result(container_.empty());
    read_semaphore_.fetch_sub(1u, MemoryOrder::release);
    read_semaphore_.notify_one();
    return result;
  }

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

  // waiting for the completion of all exectuted read operation
  void WaitForReadOperations() const {
    // waiting until read_semaphore value will be 0
    for (auto old{read_semaphore_.load(MemoryOrder::relaxed)}; old != 0u;
         old = read_semaphore_.load(MemoryOrder::relaxed)) {
      read_semaphore_.wait(old, MemoryOrder::relaxed);
    }
  }

  // modify operations
public:
  void Push(const_reference value) {
    ScopedLock<api::Mutex> lock(mutex_); // disable other modifications
    // disable new read operations
    want_to_modify_flag_.test_and_set(MemoryOrder::acquire);
    WaitForReadOperations();
    container_.push_back(value);
    CorrectInsertion(); // correct insert position with priority
    // enable new read operations
    want_to_modify_flag_.clear(MemoryOrder::release);
    want_to_modify_flag_.notify_all(); // notify read ops
    rw_sync_cvar_.notify_one();
  }

  void Push(value_type &&value) {
    ScopedLock<api::Mutex> lock(mutex_); // disable other modifications
    // disable new read operations
    want_to_modify_flag_.test_and_set(MemoryOrder::acquire);
    WaitForReadOperations();
    container_.push_back(std::move(value));
    CorrectInsertion(); // correct insert position with priority
    want_to_modify_flag_.clear(MemoryOrder::release); // enable read ops
    want_to_modify_flag_.notify_all();
    rw_sync_cvar_.notify_all();
  }

  template <class... Args> void Emplace(Args &&...args) {
    ScopedLock<api::Mutex> lock(mutex_); // disable other modifications
    // there is no opportunity to return correct result of insert operations
    // because of elements reordering

    // disable new read operations
    want_to_modify_flag_.test_and_set(MemoryOrder::acquire);
    WaitForReadOperations();
    container_.emplace_back(std::forward<Args>(args)...);
    CorrectInsertion();
    want_to_modify_flag_.clear(MemoryOrder::release); // enable read ops
    want_to_modify_flag_.notify_all();
    rw_sync_cvar_.notify_all();
  }

  value_type Pop() {
    UniqueLock<Mutex> lock(mutex_); // disable other modification
    while (container_.empty()) {
      rw_sync_cvar_.wait(lock);
    }
    // disable new read operations
    want_to_modify_flag_.test_and_set(MemoryOrder::acquire);
    WaitForReadOperations();
    auto last{std::move(container_.front())};
    container_.pop_front();
    want_to_modify_flag_.clear(MemoryOrder::release); // enable read ops
    want_to_modify_flag_.notify_all();
    return last;
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
