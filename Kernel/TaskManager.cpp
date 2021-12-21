#include "TaskManager.hpp"

// FIXME: stub
namespace impl {
api::String GetPureSignature(const api::String& full_sig) { return full_sig; }
}  // namespace impl

namespace kernel {
TaskManager::TaskManager() {}

void TaskManager::AddModule(ModuleDescriptor* md) {
  api::ScopedLock<api::SharedMutex> lock(modules_mutex_);
  modules_.push_back(md);
}

void TaskManager::EraseModule(const ModuleDescriptor* md) {
  api::ScopedLock<api::SharedMutex> lock(modules_mutex_);
  for (auto iter{modules_.begin()}, end{modules_.end()}; iter != end; ++iter) {
    if (md == *iter) {
      modules_.erase(iter);
      return;
    }
  }
}

void TaskManager::PushTask(const api::TaskWrapper& task) {
  tasks_queue_.Push(task);
}

// FIXME: stub
void TaskManager::FillConnectionsTable() noexcept(false) {}

bool TaskManager::SendNextTask() noexcept(false) {
  if (tasks_queue_.Empty()) {
    return false;
  }

  api::TaskWrapper task{tasks_queue_.Pop()};
  decltype(auto) real_task{task.GetTask()};
  api::String signal{impl::GetPureSignature(real_task->GetCausedSignal())};
  real_task->SetCausedSignal(signal);  // erase module id from signature

  // connected to this signal slots
  connections_mutex_.lock_shared();
  decltype(auto) slots{
      connections_signatures_.at(real_task->GetCausedSignal())};
  connections_mutex_.unlock_shared();
  real_task->SetNumOfAcceptors(
      static_cast<unsigned char>(slots.size()));  // number of slots
  for (auto& pair : slots) {
    task.SetTarget(pair.first);  // target slot signature
    pair.second->module->PushTask(
        std::move(task));  // adding task to module queue
  }
  return true;
}

ModuleDescriptor* TaskManager::FindDescriptor(const api::String& id) {
  api::SharedLockGuard<api::SharedMutex> lock(modules_mutex_);
  for (auto element : modules_) {
    if (element->module->GetId() == id) {
      return element;
    }
  }
  return nullptr;
}
}  // namespace kernel
