#include "ThreadManager.hpp"

namespace impl {
namespace impl_details {
// is thread could be deleted
bool IsDeleteAvailable(const api::DeferThread *thread) {
  return !thread->Joinable() && thread->NumberOfReferences() == 0;
}
}  // namespace impl_details
}  // namespace impl

namespace kernel {
using namespace impl::impl_details;
using namespace api;

ThreadManager::~ThreadManager() noexcept {
  for (auto &entry : threads_) {
    delete entry.second;
  }
}

void ThreadManager::DeleteThread(const api::ThreadId id) {
  decltype(auto) thread{threads_.at(id)};
  threads_.erase(id);

  thread->Close();
  // delete thread if it is no longer active
  if (IsDeleteAvailable(thread)) {
    delete thread;
  } else {
    closed_threads_.push_back(thread);
  }
}

OperationResult ThreadManager::ManageClosedThread() {
  if (closed_threads_.empty()) {
    return OperationResult::kNone;
  }

  for (auto thread : closed_threads_) {
    if (IsDeleteAvailable(thread)) {
      delete thread;
      return OperationResult::kSuccess;
    }
  }

  return OperationResult::kFail;  // if all threads are busy now
}

const api::ThreadSignals volatile &ThreadManager::GetThreadSignalsReference(
    const api::ThreadId id) noexcept(false) {
  return threads_.at(id)->GetSignals();
}

bool ThreadManager::SetKillSignal(const api::ThreadId id) noexcept {
  try {
    threads_.at(id)->SetSignal(api::ThreadSignal::kExit);
    return true;
  } catch (std::out_of_range) {
    return false;
  }
}

bool ThreadManager::SetSuspendSignal(const api::ThreadId id) noexcept {
  try {
    threads_.at(id)->SetSignal(api::ThreadSignal::kSuspend);
    return true;
  } catch (std::out_of_range) {
    return false;
  }
}

bool ThreadManager::ResumeThread(const api::ThreadId id) noexcept {
  try {
    threads_.at(id)->Activate();
    return true;
  } catch (std::out_of_range) {
    return false;
  }
}

void ThreadManager::SuspendThisThread(const api::ThreadId *id_hint) noexcept(
    false) {
  api::ThreadId id;
  if (id_hint) {
    id = *id_hint;
  } else {
    id = api::GetId();
  }
  threads_.at(id)->DeactivateCallerThread();
}

bool ThreadManager::UnsetSignal(const api::ThreadId id,
                                api::ThreadSignal signal) noexcept {
  try {
    threads_.at(id)->UnsetSignal(signal);
    return true;
  } catch (std::out_of_range) {
    return false;
  }
}
}  // namespace kernel
