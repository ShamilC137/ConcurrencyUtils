#ifndef APPLICATION_API_DATASTRUCTURES_TASK_HPP_
#define APPLICATION_API_DATASTRUCTURES_TASK_HPP_

// current project
#include "../../Config.hpp"
#include "../../ImplDetails/TaskDetails.hpp"
#include "Containers/String.hpp"
#include "Multithreading/Mutex.hpp"
#include "Multithreading/ScopedLock.hpp"

// STL
#include <tuple>

/// <summary>
///   This file contains Task class which encapsulate arguments for
///   intermodule communication
/// </summary>

namespace impl {
namespace impl_details {
template <class... Args>
class Arguments {
 public:
  using Tuple = std::tuple<Args...>;

  Arguments(Args &&...args) : data_{std::forward<Args>(args)...} {}

 private:
  template <class T, std::size_t... Seq>
  void UnpackHelper(T &where, std::index_sequence<Seq...>) {
    (where.push_back(std::forward<Args>(std::get<Seq>(data_))), ...);
  }

  template <class T, std::size_t... Seq>
  void UnpackHelper(T &where, std::index_sequence<Seq...>) const {
    (where.push_back(
         std::forward<std::add_const_t<Args>>(std::get<Seq>(data_))),
     ...);
  }

 public:
  // Extracts the arguments of its tuple to container. Container must have
  // "push_back" method
  template <class Container>
  void Unpack(Container &where) {
    constexpr auto size{std::tuple_size_v<Tuple>};
    UnpackHelper(where, std::make_index_sequence<size>{});
  }

  // Same as previous one but add const qualifier to tuple tupes
  template <class T>
  void Unpack(T &where) const {
    constexpr auto size{std::tuple_size_v<Tuple>};
    UnpackHelper(where, std::make_index_sequence<size>{});
  }

 private:
  template <class Container, class Inserter, std::size_t... Indexes>
  void UnpackHelper(Container &where, Inserter inserter,
                    std::index_sequence<Indexes...>) {
    ((inserter(where, std::forward<Args>(std::get<Indexes>(data_)))), ...);
  }

  template <class Container, class Inserter, std::size_t... Indexes>
  void UnpackHelper(Container &where, Inserter inserter,
                    std::index_sequence<Indexes...>) const {
    ((inserter(where,
               std::forward<std::add_const_t<Args>>(std::get<Indexes>(data_)))),
     ...);
  }

 public:
  // Extracts tuple parameters and adds them to container using the inserter
  template <class Container, class Inserter>
  void Unpack(Container &where, Inserter inserter) {
    constexpr auto size{sizeof...(Args)};
    UnpackHelper(where, inserter, std::make_index_sequence<size>{});
  }

  // Same as previous one but adds the const qualifier to tuple parameters
  template <class Container, class Inserter>
  void Unpack(Container &where, Inserter inserter) const {
    constexpr auto size{sizeof...(Args)};
    UnpackHelper(where, inserter, std::make_index_sequence<size>{});
  }

  [[nodiscard]] inline Tuple &GetTuple() noexcept { return data_; }

  // Return data tuple
  [[nodiscard]] inline const Tuple &GetTuple() const noexcept { return data_; }

 private:
  Tuple data_;
};

// RetTypeResolution is the helper type for ReturnTask. Task return value is
// stored here. It introduce "SetResult" and "GetResult" function which adopted
// for its underlying types. If target thread still not provide result of its
// work, then "GetResult" will wait for target thread exit.

// Specialization type: rvalue ref or prvalue.
// May throw
// Allocates memory for its result field. Using "move" to return result value.
// BaseTask is used for multithreading safety insurance. If the callee thread
// is working while the caller thread trying to a get result, caller will be
// blocked until the callee exits.
template <class RetType>
class RetTypeResolution {
 public:
  using ValueType = std::decay_t<RetType>;
  using BasePointer = BaseTask *;
  using PointerType = ValueType *;
  using Alloc = std::allocator<ValueType>;

 public:
  inline RetTypeResolution(const BasePointer &base_task) noexcept
      : safety_object_{base_task} {}

  // Throws: api::Deadlock
  // Set the result value with given. Allocates memory for result value.
  void SetResult(RetType &&value) noexcept(false) {
    Alloc allocator{};
    try {
      value_ = allocator.allocate(1);
    } catch (...) {
      throw api::BrokenReturnTask("Can not allocate memory for return value");
    }
    std::allocator_traits<Alloc>::construct(allocator, value_,
                                            std::move(value));
  }

  // Return result value (moving it)
  // Throws: api::Deadlock
  RetType GetResult() noexcept(false) {
    if (safety_object_->IsWaitable()) {
      safety_object_->Wait();  // throws
    }
    if (!value_) {
      throw api::BrokenReturnTask("Result value is empty");
    }
    return std::move(*value_);
  }

  // Deallocate memory for value
  ~RetTypeResolution() {
    if (value_) {
      Alloc allocator{};
      std::allocator_traits<Alloc>::destroy(allocator, value_);
      allocator.deallocate(value_, 1);
    }
  }

 private:
  PointerType value_{};
  BasePointer safety_object_;  // ensure multithreading safety
};

// Specialization type: lvalue ref
// Adopts pointer to result value. If value is expired after adoptation - UB.
template <class RetType>
class RetTypeResolution<RetType &> {
 public:
  using PointerType = RetType *;
  using BasePointer = BaseTask *;

 public:
  inline RetTypeResolution(const BasePointer &base_task) noexcept
      : safety_object_{base_task} {}

  // Adopt pointer to value.
  inline void SetResult(RetType &val) noexcept { value_ = std::addressof(val); }

  // Return reference to value. If value is expired - UB.
  // Throws: api::Deadlock
  RetType &GetResult() const noexcept(false) {
    if (safety_object_->IsWaitable()) {
      safety_object_->Wait();  // throws
    }
    return *value_;
  }

 private:
  PointerType value_{};
  BasePointer safety_object_;  // ensure multithreading safety
};

// Specialization type: pointer
// Adopts pointer to result value. If value is expired after adoptation - UB.
template <class RetType>
class RetTypeResolution<RetType *> {
 public:
  using PointerType = RetType *;
  using BasePointer = BaseTask *;

 public:
  inline RetTypeResolution(const BasePointer &base_task) noexcept
      : safety_object_{base_task} {}

  // Adopts pointer to value.
  inline void SetResult(RetType *val) noexcept { value_ = val; }

  // Return reference to value. If value is expired - UB.
  // Throws: api::Deadlock
  RetType *GetResult() noexcept(false) {
    if (safety_object_->IsWaitable()) {
      safety_object_->Wait();  // throws
    }
    return value_;
  }

 private:
  PointerType value_{};
  BasePointer safety_object_;  // ensure multithreading safety
};
}  // namespace impl_details
}  // namespace impl
namespace api {
/// <summary>
/// Task is the class that encapsulates parameters for slots.
/// Arguments are contained in the tuple which can be unpacked in several ways:
/// 1) Extract tuple itself with "GetTuple" method
/// 2) Call "Unpack" and pass container to this method to extract the arguments
/// to. Also the "inserter" could be given then "Unpack" will call inserter to
/// add new arguments to container (i.e. inserter(container,
/// std::forward(arg))). The task is used by the Slot which encapsulate the
/// argument extraction work so the user only has to pass pointer to the Task to
/// the Slot. If is_blocking_task flag is setted, Task will wait for all threads
/// routine completion.
/// Warning: few thread could read data at the same time.
/// </summary>
/// <typeparam name="...Args">
///   Target slot parameters types
/// </typeparam>
template <class... Args>
class Task : public impl::BaseTask {
 public:
  using Base = impl::BaseTask;
  using Tuple = typename impl::impl_details::Arguments<Args...>::Tuple;
  using Arguments = impl::impl_details::Arguments<Args...>;

 public:
  /// <param name="signal_sig">
  /// signal full signature
  /// </param>
  /// <param name="priority">
  ///   used for task compare
  /// </param>
  /// <param name="...args">
  ///   target slot arguments
  /// </param>
  Task(api::String signal_sig, TaskPriority priority,
       impl::ForceExplicitTypeT<Args>... args)
      : Task(signal_sig, priority, nullptr, std::forward<Args>(args)...) {}

  ~Task() noexcept override {
    try {
      Base::Wait();  // throws
      if (args_) {
        ClearArguments();
      }
    } catch (...) {
      if (args_) {
        ClearArguments();
      }
      assert(false && "Deadlock during task destruction");
    }
  }

 protected:
  Task(api::String caused_signal_sig, TaskPriority priority, const int *retid,
       impl::ForceExplicitTypeT<Args>... args)
      : Base(caused_signal_sig, priority,
             impl::IdSequence<Args...>::CreateIdSequence(), retid),
        args_{new Arguments{std::forward<Args>(args)...}} {}

 public:
  /// <summary>
  ///   Extracts task arguments to given container with push_back
  /// </summary>
  /// <typeparam name="Container">
  ///   Container type
  /// </typeparam>
  /// <param name="where">
  ///   Extraction place
  /// </param>
  template <class Container>
  void Unpack(Container &where) {
    args_->Unpack(where);
  }

  /// <summary>
  ///   Extracts task arguments to given container with push_back
  /// </summary>
  /// <typeparam name="Container">
  ///   Container type
  /// </typeparam>
  /// <param name="where">
  ///   Extraction place
  /// </param>
  template <class Container>
  void Unpack(Container &where) const {
    args_->Unpack(where);
  }

  /// <summary>
  ///   Extracts arguments from task and pushes it to given container using
  ///   given inserter.
  /// </summary>
  /// <typeparam name="Container">
  ///   Container type
  /// </typeparam>
  /// <typeparam name="Inserter">
  ///   Inserter type
  /// </typeparam>
  /// <param name="where">
  ///   Extraction place
  /// </param>
  /// <param name="inserter">
  ///   Inserts given argument to container
  /// </param>
  template <class Container, class Inserter>
  void Unpack(Container &where, Inserter inserter) {
    args_->Unpack(where, inserter);
  }

  /// <summary>
  ///   Extracts arguments from task and pushes it to given container using
  ///   given inserter.
  /// </summary>
  /// <typeparam name="Container">
  ///   Container type
  /// </typeparam>
  /// <typeparam name="Inserter">
  ///   Inserter type
  /// </typeparam>
  /// <param name="where">
  ///   Extraction place
  /// </param>
  /// <param name="inserter">
  ///   Inserts given argument to container
  /// </param>
  template <class Container, class Inserter>
  void Unpack(Container &where, Inserter inserter) const {
    args_->Unpack(where, inserter);
  }

  /// <returns>
  ///   Tuple of arguments
  /// </returns>
  [[nodiscard]] inline Tuple &GetTuple() noexcept { return args_->GetTuple(); }

  /// <returns>
  ///   Tuple of arguments
  /// </returns>
  [[nodiscard]] inline const Tuple &GetTuple() const noexcept {
    return args_->GetTuple();
  }

  /// <summary>
  ///   Deletes arguments (if present). Safe for multithreading.
  /// </summary>
  void ClearArguments() override {
    ScopedLock<Mutex> lock(args_mutex_, boost::interprocess::try_to_lock);
    if (lock.owns()) {
      if (args_) {
        delete args_;
        args_ = nullptr;
      }
    }
  }

  // fields
 private:
  Arguments *args_;
  Mutex args_mutex_;
};

/// <summary>
///   Task that allows to return value from target function. Assumed that result
///   of associated with this task slot's routine will be get with "GetResult"
///   which wait for target thread complete the task. If it this did not happen,
///   it means that an error has occured and an exception will be thrown. If
///   ReturnTask is connected with few slots - UB, debug mode will throw
///   exception, release mode will write message about error in error log.
/// </summary>
/// <typeparam name="ReturnType">
///   Target function return type
/// </typeparam>
/// <typeparam name="...Args">
///   Target function arguments types
/// </typeparam>
template <class ReturnType, class... Args>
class ReturnTask : public Task<Args...>,
                   public impl::impl_details::RetTypeResolution<ReturnType> {
  using Base = Task<Args...>;
  using RetBase = impl::impl_details::RetTypeResolution<ReturnType>;

 public:
  /// <param name="signal_sig">
  ///   signal full signature (i.e. with module id)
  /// </param>
  /// <param name="priority">
  ///   used for task compare
  /// </param>
  /// <param name="...args">
  ///   target slot arguments
  /// </param>
  ReturnTask(String signal_sig, TaskPriority priority,
             impl::ForceExplicitTypeT<Args>... args)
      : Base(signal_sig, priority, &impl::TypeID<ReturnType>::id,
             std::forward<Args>(args)...),
        RetBase(this) {}

  ~ReturnTask() noexcept override {
    assert(!Base::IsWaitable() &&
           "Callee thread still working");  // if true, it means that GetResult
                                            // was not called. Such condition
                                            // treated as unacceptable.
  }

  /// <summary>
  ///   Sets number of task acceptors (i.e. slots). Cannot be more than one.
  /// </summary>
  /// <param name="nacceptors">
  ///   Number of acceptors
  /// </param>
  /// <param name="order">
  ///   store() memory order
  /// </param>
  virtual inline void SetNumOfAcceptors(
      const unsigned char nacceptors,
      api::MemoryOrder order =
          api::MemoryOrder::release) noexcept(false) override {
    if (nacceptors > 1) {
      throw BrokenReturnTask("Return task cannot have more than one acceptor.");
    } else {
      Base::SetNumOfAcceptors(nacceptors, order);
    }
  }
};

}  // namespace api

#endif  // !APPLICATION_API_DATASTRUCTURES_TASK_HPP_
