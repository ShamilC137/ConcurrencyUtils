#include "AbstractModule.hpp"

namespace impl {
// ctors

AbstractModule::AbstractModule(const api::String &mid) : id_{mid} {}

// getters

api::Vector<api::String> AbstractModule::GetSlotsSignatures() const {
  api::Vector<api::String> result(slots_.size());
  std::size_t index{};
  for (const auto &pair : slots_) {
    result[index++] = pair.first;
  }
  return result;
}

// modifiers

[[nodiscard]] api::TaskWrapper AbstractModule::TryExtractTask() noexcept(
    false) {
  try {
    return tasks_queue_.TryPop();
  } catch (const api::PopFailed &) {
    throw;
  } catch (...) {
    assert(false && "Panic: TryExtractTask, unexpected exception");
    throw;
  }
}

ThreadResourceErrorStatus AbstractModule::ExecuteNextTask() noexcept(false) {
  auto task{tasks_queue_.Pop()};  // throws: api::PopFailed
  return ExecuteTask(std::move(task));
}

void AbstractModule::ExecuteNextTask(api::ForceSlotCall) noexcept(false) {
  auto task{tasks_queue_.Pop()};  // throws: api::PopFailed
  ExecuteTask(std::move(task), {});
}

void AbstractModule::ExecuteTask(api::TaskWrapper task,
                                 api::ForceSlotCall) noexcept(false) {
  // throws: std::out_of_range
  auto slot_ptr{slots_.at(task.GetTarget()).get()};
  (*slot_ptr)(task);  // throws: api::BadSlotCall
}

ThreadResourceErrorStatus AbstractModule::ExecuteTask(
    api::TaskWrapper task) noexcept(false) {
  auto slot_ptr{
      slots_.at(task.GetTarget()).get()};  // throws: std::out_of_range
  if (slot_ptr->Execute()) {
    return ThreadResourceErrorStatus::kBusy;
  }

  // throws: api::BadSlotCall
  (*slot_ptr)(task);
  slot_ptr->Release();
  return ThreadResourceErrorStatus::kOk;
}
}  // namespace impl
