#include "AbstractModule.hpp"

namespace impl {
// ctors

AbstractModule::AbstractModule(const api::String &mid) : id_{mid} {}

AbstractModule::~AbstractModule() noexcept {}

// getters

api::Vector<api::String> AbstractModule::GetSlotsSignatures() const {
  api::SharedLockGuard<api::SharedMutex> lock(slots_mutex_);
  api::Vector<api::String> result(slots_.size());
  std::size_t index{};
  for (const auto &pair : slots_) {
    result[index++] = pair.first;
  }
  return result;
}

bool AbstractModule::AddSlotToTable(const api::String &slot_sig,
                                    impl::BaseSlot *slot) {
  slots_mutex_.lock();
  const auto is_inserted{slots_.try_emplace(slot_sig, slot).second};
  slots_mutex_.unlock();
  if (!is_inserted) {
    delete slot;
  }
  return is_inserted;
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
  ExecuteTask(std::move(task), api::ForceSlotCall{});
}

void AbstractModule::ExecuteTask(api::TaskWrapper task,
                                 api::ForceSlotCall) const noexcept(false) {
  // throws: std::out_of_range
  slots_mutex_.lock_shared();
  auto slot_ptr{slots_.at(task.GetTarget()).get()};
  slots_mutex_.unlock_shared();
  (*slot_ptr)(task);  // throws: api::BadSlotCall
}

// FIXME: implement
void AbstractModule::SetSlotsPriorities() noexcept(false) {
  // assert(slot_priority != 0 && slot_priority >= -1)
}

ThreadResourceErrorStatus AbstractModule::ExecuteTask(
    api::TaskWrapper task) const noexcept(false) {
  slots_mutex_.lock_shared();
  auto slot_ptr{
      slots_.at(task.GetTarget()).get()};  // throws: std::out_of_range
  slots_mutex_.unlock_shared();
  if (slot_ptr->Execute()) {
    return ThreadResourceErrorStatus::kBusy;
  }

  // throws: api::BadSlotCall
  (*slot_ptr)(task);
  slot_ptr->Release();
  return ThreadResourceErrorStatus::kOk;
}
}  // namespace impl
