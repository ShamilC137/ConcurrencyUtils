#include "TaskManager.hpp"

namespace impl {
// FIXME: stub
api::Pair<api::String, api::String> GetComponentsStub(api::String full_sig) {
  return {};
}
}  // namespace impl

namespace kernel {
TaskManager::TaskManager() {}

void TaskManager::AddModule(ModuleDescriptor* md) { modules_.push_back(md); }

void TaskManager::DeleteModule(const ModuleDescriptor* md) {
  for (auto iter{modules_.begin()}, end{modules_.end()}; iter != end; ++iter) {
    if (md == *iter) {
      modules_.erase(iter);
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
  api::Pair signal{impl::GetComponentsStub(real_task->GetCausedSignal())};
  real_task->SetCausedSignal(signal.second);  // erase module id from signature

  // conencted to this signal slots
  decltype(auto) slots{
      connections_signatures_.at(real_task->GetCausedSignal())};
  real_task->SetNumOfAcceptors(slots.size());  // number of slots
  for (auto pair : slots) {
    task.SetTarget(pair.first);           // target slot signature
    pair.second->module->PushTask(task);  // adding task to module queue
  }
}

ModuleDescriptor* TaskManager::FindDescriptor(const api::String& id) {
  for (auto element : modules_) {
    if (element->module->GetId() == id) {
      return element;
    }
  }
  return nullptr;
}
}  // namespace kernel
