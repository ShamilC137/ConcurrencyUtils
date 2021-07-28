#ifndef APPLICATION_API_MULTITHREADING_UNBOUNDEDBLOCKINGQUEUE_HPP_
#define APPLICATION_API_MULTITHREADING_UNBOUNDEDBLOCKINGQUEUE_HPP_

// current project
#include "Atomics.hpp"
#include "ConditionVariable.hpp"
#include "Mutex.hpp"
#include "ScopedLock.hpp"
#include "UniqueLock.hpp"

#include "../Containers/Deque.hpp"

namespace api {
// This container represents unbounded priority queue, which allows have single
// writter and multiple readers. All "Push" operation only adding new element to
// the container, insert position is dependent from priority. By default, new
// element inserted to the end of container and then moved in right position.
// WARNING: careless usage of this container methods to implement other its
// methods may cause deadlock!
// WARNING 2: deadlock also could be reached if all threads will try to   
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

public:
  // Return first element in queue.
  [[nodiscard]] reference Front() noexcept(noexcept(container_.front())) {
    read_flag_.test_and_set(
        boost::memory_order::acquire); // disable write operations
    decltype(auto) result(container_.front());
    read_flag_.clear(boost::memory_order::release); // enable write operations
    read_flag_.notify_one();
    return result;
  }

  [[nodiscard]] const_reference Front() const
      noexcept(noexcept(container_.front())) {
    read_flag_.test_and_set(
        boost::memory_order::acquire); // disable write operations
    decltype(auto) result(container_.front());
    read_flag_.clear(boost::memory_order::release); // enable write operations
    read_flag_.notify_one();
    return result;
  }

  [[nodiscard]] reference Back() noexcept(noexcept(container_.back())) {
    read_flag_.test_and_set(
        boost::memory_order::acquire); // disable write operations
    decltype(auto) result(container_.back());
    read_flag_.clear(boost::memory_order::release); // enable write operations
    read_flag_.notify_one();
    return result;
  }

  [[nodiscard]] const_reference Back() const
      noexcept(noexcept(container_.back())) {
    read_flag_.test_and_set(
        boost::memory_order::acquire); // disable write operations
    decltype(auto) result(container_.back());
    read_flag_.clear(boost::memory_order::release); // enable write operations
    read_flag_.notify_one();
    return result;
  }

  [[nodiscard]] bool Empty() const noexcept(noexcept(container_.empty())) {
    read_flag_.test_and_set(
        boost::memory_order::acquire); // disable write operations
    decltype(auto) result(container_.empty());
    read_flag_.clear(boost::memory_order::release); // enable write operations
    read_flag_.notify_one();
    return result;
  }

  [[nodiscard]] size_type Size() const noexcept(noexcept(container_.size())) {
    read_flag_.test_and_set(
        boost::memory_order::acquire); // disable write operations
    decltype(auto) result(container_.size());
    read_flag_.clear(boost::memory_order::release); // enable write operations
    read_flag_.notify_one();
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

public:
  void Push(const_reference value) {
    while (read_flag_.test(boost::memory_order::relaxed)) {
      read_flag_.wait(true, boost::memory_order::relaxed);
    }
    ScopedLock<api::Mutex> lock(mutex_); // disable other writes
    container_.push_back(value);
    CorrectInsertion(); // correct insert position with priority
    rw_sync_cvar_.notify_one();
  }

  void Push(value_type &&value) {
    while (read_flag_.test(boost::memory_order::relaxed)) {
      read_flag_.wait(true, boost::memory_order::relaxed);
    }
    ScopedLock<api::Mutex> lock(mutex_); // disable other writes
    container_.push_back(std::move(value));
    CorrectInsertion(); // correct insert position with priority
    rw_sync_cvar_.notify_one();
  }

  template <class... Args> void Emplace(Args &&...args) {
    while (read_flag_.test(boost::memory_order::relaxed)) {
      read_flag_.wait(true, boost::memory_order::relaxed);
    }
    ScopedLock<api::Mutex> lock(mutex_); // disable other writes
    // there is no opportunity to return correct result of insert operations
    // because of elements reordering
    container_.emplace_back(std::forward<Args>(args)...);
    CorrectInsertion();
    rw_sync_cvar_.notify_one();
  }

  value_type Pop() {
    while (read_flag_.test(boost::memory_order::relaxed)) {
      read_flag_.wait(true, boost::memory_order::relaxed);
    }
    UniqueLock<Mutex> lock(mutex_);
    while (container_.empty()) {
      rw_sync_cvar_.wait(lock);
    }

    auto last{std::move(container_.front())};
    container_.pop_front();
    return last;
  }

  // fields
private:
  // mutex for read-write sync
  Mutex mutex_;
  // if enabled, then write is disallowed;
  mutable AtomicFlag read_flag_;
  // conditional variable for read-write sync
  ConditionVariable rw_sync_cvar_;
  // queue
  Container container_;
};
} // namespace api

#endif // !APPLICATION_API_MULTITHREADING_UNBOUNDEDBLOCKINGQUEUE_HPP_
