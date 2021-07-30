#ifndef APPLICATION_API_DATASTRUCTURES_SLOT_HPP_
#define APPLICATION_API_DATASTRUCTURES_SLOT_HPP_

// current project
#include "Task.hpp"
#include "../../ImplDetails/SlotDetails.hpp"

// STD
#include <functional>

namespace api {
template <class ReturnType, class... Args>
class Slot : public impl::BaseSlot {
 public:
  using Base = impl::BaseSlot;

 public:
  Slot(impl::ForceExplicitTypeT<ReturnType(Args...)> *function)
      : Base(impl::IDSequence<Args...>::CreateIDSequence()), func_{function} {}

 private:
  template <std::size_t... Indexes>
  void CallHelper(impl::BaseTask *task, std::index_sequence<Indexes...>) {
    Task<Args...> *ptr{static_cast<Task<Args...> *>(task)};
    decltype(auto) parameters{ptr->GetTuple()};
    func_(std::forward<Args>(std::get<Indexes>(parameters))...);
  }

 protected:
  virtual void RealCall(impl::BaseTask *task) noexcept(false) override {
    if (task->GetIDSequencePtr() != Base::GetIDSequencePtr()) {
      throw impl::BadSlotCall(
          "Task and slot parameters types are incompatible");
    }

    CallHelper(task, std::make_index_sequence<sizeof...(Args)>{});
  }

 private:
  std::function<ReturnType(Args...)> func_;
};

}  // namespace api
#endif // !APPLICATION_API_DATASTRUCTURES_SLOT_HPP_
