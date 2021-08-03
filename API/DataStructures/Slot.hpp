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
  virtual void RealCall(impl::BaseTask *task) noexcept(false) override {
    if (task->GetIDSequencePtr() != Base::GetIDSequencePtr()) {
      throw api::BadSlotCall("Task and slot parameters types are incompatible");
    }

    if (task->IsMustReturn()) {
      if (task->GetRetIDPtr() != Base::GetRetIDPtr()) {
        throw api::BadSlotCall("Task and slot return types are incompatible");
      } else {
        // since code above may throw exception this check appear in both
        // situations
        if (const auto priority{Base::GetPriority()}; priority != -1) {
          task->Wait(priority);
        }
        ReturningCall(static_cast<ReturnTask<ReturnType, Args...> *>(task),
                      std::make_index_sequence<sizeof...(Args)>{});
      }
    } else {
      if (const auto priority{Base::GetPriority()}; priority != -1) {
        task->Wait(priority);
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
