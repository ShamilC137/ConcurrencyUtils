#ifndef APPLICATION_API_DATASTRUCTURES_TASK_HPP_
#define APPLICATION_API_DATASTRUCTURES_TASK_HPP_

// current project
#include "../../Config.hpp"
#include "../../ImplDetails/TaskDetails.hpp"
#include "Containers/String.hpp"

// conditional headers
#if VALLOCATOR_USAGE
#include "../Memory/VAllocator.hpp"
#endif

// STL
// Allocator construct and destroy are deprecated in C++17 so I we have to use
// allocator_traits because there are some checks that disallow to compile
// code with these members. Use of define will not fix problem because
// other header also may use "memory" header.
#include <memory> // for allocator_traits
#include <tuple>

namespace api {
// Task wrapper that deletes task if number of references on this task becomes 0
struct TaskWrapper {
#if STL_ALLOCATOR_USAGE
  using PointerType = impl::BaseTask *;
#elif VALLOCATOR_USAGE
  using PointerType = VPtr<impl::BaseTask>;
#endif

  TaskWrapper(const PointerType &task) noexcept(false) : task{task} {}

  ~TaskWrapper() noexcept {
    if (task->DecrementNumOfRefs() == 0u) {
#if STL_ALLOCATOR_USAGE
      delete task;
#elif VALLOCATOR_USAGE
      VAllocator<impl::BaseTask>::DestroyAndDeallocateOne(task);
#endif
    }
  }

  PointerType task;
};

// Task is the class that encapsulates parameters for slots.
// Arguments are contained in the tuple which can be unpacked in several ways:
// 1) Extract tuple itself with "GetTuple" method
// 2) Call "Unpack" and pass container to this method to extract the arguments
// to. Also the "inserter" could be given then "Unpack" will call inserter to
// add new arguments to container (i.e. inserter(container, std::forward(arg))).
// The task is used by the Slot which encapsulate the argument extraction work
// so the user only has to pass pointer to the Task to the Slot.
// If is_blocking_task flag is setted, Task will wait for all threads
// routine completion.
// Warning: few thread could read data at the same time.
template <class... Args> class Task : public impl::BaseTask {
public:
  using Base = impl::BaseTask;
  using Tuple = std::tuple<Args...>;

public:
  // module_id - caller module id
  // is_blocking_task - if true task must wait for its slot complete call
  // args - target slot arguments
  Task(api::String module_id, bool is_blocking_task,
       impl::ForceExplicitTypeT<Args>... args)
      : Task(module_id, is_blocking_task, nullptr,
             std::forward<Args>(args)...) {}

  ~Task() noexcept(false) override {
    if (Base::IsBlockingTask()) {
      Base::Wait(); // throws
    }
  }

protected:
  Task(api::String module_id, bool is_blocking_task, const int *retid,
       impl::ForceExplicitTypeT<Args>... args)
      : Base(module_id, is_blocking_task,
             impl::IDSequence<Args...>::CreateIDSequence(), retid),
        data_{std::forward<Args>(args)...} {}

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
  template <class Container> void Unpack(Container &where) {
    constexpr auto size{std::tuple_size_v<Tuple>};
    UnpackHelper(where, std::make_index_sequence<size>{});
  }

  // Same as previous one but add const qualifier to tuple tupes
  template <class T> void Unpack(T &where) const {
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

  // Return data tuple
  [[nodiscard]] inline Tuple &GetTuple() noexcept { return data_; }

  // Return data tuple
  [[nodiscard]] inline const Tuple &GetTuple() const noexcept { return data_; }

  // fields
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
template <class RetType> class RetTypeResolution {
public:
  using ValueType = std::decay_t<RetType>;
#if STL_ALLOCATOR_USAGE
  using PointerType = ValueType *;
  using Alloc = std::allocator<ValueType>;
  using BasePointer = impl::BaseTask *;
#elif VALLOCATOR_USAGE
  using PointerType = VPtr<ValueType>;
  using BasePointer = VPtr<impl::BaseTask>;
  using Alloc = VAllocator<ValueType>;
#endif

public:
  inline RetTypeResolution(const BasePointer &base_task) noexcept
      : safety_object_{base_task} {}

  // Throws
  // Set the result value with given. Allocates memory for result value.
  void SetResult(RetType &&value) noexcept(false) {
    Alloc allocator{};
    try {
      value_ = allocator.allocate(1);
    } catch (...) {
      throw BrokenReturnTask("Cannot allocate memory for return value");
    }
    std::allocator_traits<Alloc>::construct(allocator, value_,
                                            std::move(value));
  }

  // Return result value (moving it)
  RetType GetResult() noexcept(false) {
    if (safety_object_->IsWaitable()) {
      safety_object_->Wait(); // throws
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
  BasePointer safety_object_; // ensure multithreading safety
};

// Specialization type: lvalue ref
// Adopts pointer to result value. If value is expired after adoptation - UB.
template <class RetType> class RetTypeResolution<RetType &> {
public:
  using PointerType = RetType *;

#if STL_ALLOCATOR_USAGE
  using BasePointer = impl::BaseTask *;
#elif VALLOCATOR_USAGE
  using BasePointer = VPtr<impl::BaseTask>;
#endif

public:
  inline RetTypeResolution(const BasePointer &base_task) noexcept
      : safety_object_{base_task} {}

  // Adopt pointer to value.
  inline void SetResult(RetType &val) noexcept { value_ = std::addressof(val); }

  // Return reference to value. If value is expired - UB.
  RetType &GetResult() const noexcept(false) {
    if (safety_object_->IsWaitable()) {
      safety_object_->Wait(); // throws
    }
    return *value_;
  }

private:
  PointerType value_{};
  BasePointer safety_object_; // ensure multithreading safety
};

// Specialization type: pointer
// Adopts pointer to result value. If value is expired after adoptation - UB.
template <class RetType> class RetTypeResolution<RetType *> {
public:
  using PointerType = RetType *;

#if STL_ALLOCATOR_USAGE
  using BasePointer = impl::BaseTask *;
#elif VALLOCATOR_USAGE
  using BasePointer = VPtr<impl::BaseTask>;
#endif

public:
  inline RetTypeResolution(const BasePointer &base_task) noexcept
      : safety_object_{base_task} {}

  // Adopts pointer to value.
  inline void SetResult(RetType *val) noexcept { value_ = val; }

  // Return reference to value. If value is expired - UB.
  RetType *GetResult() noexcept(false) {
    if (safety_object_->IsWaitable()) {
      safety_object_->Wait(); // throws
    }
    return value_;
  }

private:
  PointerType value_{};
  impl::BaseTask *safety_object_; // ensure multithreading safety
};

// Task that allows to return value from target function. Assumed that result
// of associated with this task slot's routine will be get with "GetResult"
// which wait for target thread complete the task. If it this did not happen, it
// means that an error has occured and an exception will be thrown.
// If ReturnTask is connected with few slots - UB, debug mode will throw
// exception, release mode will write message about error in error log.
template <class ReturnType, class... Args>
class ReturnTask : public Task<Args...>, public RetTypeResolution<ReturnType> {
  using Base = Task<Args...>;
  using RetBase = RetTypeResolution<ReturnType>;

public:
  // module_id - caller module id
  // args - target slot arguments
  ReturnTask(String module_id, impl::ForceExplicitTypeT<Args>... args)
      : Base(module_id, true, &impl::TypeID<ReturnType>::id,
             std::forward<Args>(args)...),
        RetBase(this) {}

  ~ReturnTask() noexcept(false) override {
    if (Base::IsWaitable()) { // if true, it means that GetResult was not
                              // called. Such condition treated as unacceptable.
      throw BrokenReturnTask("Callee thread still working");
    }
  }
};

} // namespace api

#endif // !APPLICATION_API_DATASTRUCTURES_TASK_HPP_
