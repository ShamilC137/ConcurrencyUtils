#ifndef APPLICATION_API_DATASTRUCTURES_SLOT_HPP_
#define APPLICATION_API_DATASTRUCTURES_SLOT_HPP_

// current project
#include "../../ImplDetails/SlotDetails.hpp"
#include "Task.hpp"

// STD
#include <functional>

namespace api {
// Slot is the class which allows to hide underlying function and call it
// with given tasks.
template <class ReturnType, class... Args> class Slot : public impl::BaseSlot {
public:
  using Base = impl::BaseSlot;

public:
  Slot(impl::ForceExplicitTypeT<ReturnType(Args...)> *function)
      : Base(impl::IDSequence<Args...>::CreateIDSequence(),
             &impl::TypeID<ReturnType>::id),
        func_{function} {}

  // Calls underlying function with given task.
  // throws: api::BadSlotCall
  using Base::operator();

  // Return the size in bytes of this class; used to correct deallocate object
  // of this class which captured with pointer to base.
  [[nodiscard]] inline std::size_t SizeInBytes() const noexcept override {
    return sizeof(Slot);
  }

private:
  template <std::size_t... Indexes>
  void NonReturningCall(Task<Args...> *task, std::index_sequence<Indexes...>) {
    decltype(auto) parameters{task->GetTuple()};
    func_(std::forward<Args>(std::get<Indexes>(parameters))...);
  }

  template <std::size_t... Indexes>
  void ReturningCall(ReturnTask<ReturnType, Args...> *task,
                     std::index_sequence<Indexes...>) {
    decltype(auto) parameters{task->GetTuple()};
    task->SetResult(std::forward<ReturnType>(
        func_(std::forward<Args>(std::get<Indexes>(parameters))...)));
  }

protected:
  // Calls the underlying function with parameters from task.
  // Throws: api::Deadlock, api::BadSlotCall
  // FIXME: void ReturnType stub
  virtual void RealCall(TaskWrapper &task_wrap) noexcept(false) override {
    decltype(auto) task{task_wrap.GetTask()};
    if (task->GetIDSequencePtr() != Base::GetIDSequencePtr()) {
      throw api::BadSlotCall("Task and slot parameters types are incompatible");
    }

    if (task->IsMustReturn()) {
      if (task->GetRetIDPtr() != Base::GetRetIDPtr()) {
        throw api::BadSlotCall("Task and slot return types are incompatible");
      } else {
        // since code above may throw exception this check appear in both
        // situations
        if (const auto priority{Base::GetPriority(task_wrap.GetCausedSignal())};
            priority != -1) {
          // throws: api::Deadlock
          task->Wait(static_cast<unsigned char>(priority));
        }
        if constexpr (std::is_same_v<ReturnType, void>) {
          throw BadSlotCall("Bad call with void return type");
        } else {
          ReturningCall(static_cast<ReturnTask<ReturnType, Args...> *>(task),
                        std::make_index_sequence<sizeof...(Args)>{});
        }
      }
    } else {
      if (const auto priority{Base::GetPriority(task_wrap.GetCausedSignal())};
          priority != -1) {
        // throws: api::Deadlock
        task->Wait(static_cast<unsigned char>(priority));
      }
      NonReturningCall(static_cast<Task<Args...> *>(task),
                       std::make_index_sequence<sizeof...(Args)>{});
    }

    task->NotifyAboutComplete();
  }

private:
  std::function<ReturnType(Args...)> func_;
};

} // namespace api
#endif // !APPLICATION_API_DATASTRUCTURES_SLOT_HPP_
