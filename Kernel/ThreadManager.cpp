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

ThreadManager::~ThreadManager() noexcept { UnsafeDeleteAll(); }

api::DeferThreadWrapper ThreadManager::AddThread(api::DeferThread *thread) {
  api::ScopedLock<api::SharedMutex> lock(threads_mutex_);
  if (threads_.find(thread->GetId()) == threads_.end()) {
    threads_[thread->GetId()] = thread;
  }
  lock.unlock();
  return api::DeferThreadWrapper(thread);
}

void ThreadManager::DeleteThread(const api::ThreadId id) noexcept(false) {
  api::ScopedLock<api::SharedMutex> lock(threads_mutex_);
  decltype(auto) thread{threads_.at(id)};
  threads_.erase(id);
  lock.unlock();

  thread->Close();
  // delete thread if it is no longer active
  if (IsDeleteAvailable(thread)) {
    delete thread;
  } else {
    api::ScopedLock<api::Mutex> lock(closed_threads_mutex_);
    closed_threads_.push_back(thread);
  }
}

OperationResult ThreadManager::ManageClosedThread() {
  api::ScopedLock<api::Mutex> lock(closed_threads_mutex_);
  if (closed_threads_.empty()) {
    return OperationResult::kNone;
  }

  for (auto iter = closed_threads_.begin(), end = closed_threads_.end();
       iter != end; ++iter) {
    if (IsDeleteAvailable(*iter)) {
      delete *iter;
      closed_threads_.erase(iter);
      return OperationResult::kSuccess;
    }
  }
  return OperationResult::kFail;  // if all threads are busy now
}

void ThreadManager::StartAll() {
  api::SharedLockGuard<api::SharedMutex> lock(threads_mutex_);
  for (auto &pair : threads_) {
    pair.second->Activate();
  }
}

void ThreadManager::UnsafeDeleteAll() {
  for (auto &entry : threads_) {
    entry.second->Close();
    if (entry.second->Joinable()) {
      entry.second->Join();
    }
    delete entry.second;
  }

  for (auto &thread : closed_threads_) {
    if (thread->Joinable()) {
      thread->Join();
    }
    delete thread;
  }
}

void ThreadManager::ForceDeleteAll() {
  api::ScopedLock<api::SharedMutex> lock(threads_mutex_);
  UnsafeDeleteAll();
}

[[nodiscard]] api::ThreadSignals ThreadManager::GetThreadSignals(
    const api::ThreadId id) const noexcept(false) {
  api::SharedLockGuard<api::SharedMutex> lock(threads_mutex_);
  return threads_.at(id)->GetSignals();
}

bool ThreadManager::SendKillSignal(const api::ThreadId id) noexcept {
  try {
    api::SharedLockGuard<api::SharedMutex> lock(threads_mutex_);
    threads_.at(id)->SetSignal(api::ThreadSignal::kExit);
    return true;
  } catch (std::out_of_range) {
    return false;
  }
}

bool ThreadManager::SetSuspendSignal(const api::ThreadId id) noexcept {
  try {
    api::SharedLockGuard<api::SharedMutex> lock(threads_mutex_);
    threads_.at(id)->SetSignal(api::ThreadSignal::kSuspend);
    return true;
  } catch (std::out_of_range) {
    return false;
  }
}

bool ThreadManager::ResumeThread(const api::ThreadId id) noexcept {
  try {
    api::SharedLockGuard<api::SharedMutex> lock(threads_mutex_);
    threads_.at(id)->Activate();
    return true;
  } catch (std::out_of_range) {
    return false;
  }
}

void ThreadManager::SuspendThisThread(
    const api::ThreadId *const id_hint) noexcept(false) {
  api::ThreadId id;
  if (id_hint) {
    id = *id_hint;
  } else {
    id = api::GetId();
  }
  api::SharedLockGuard<api::SharedMutex> lock(threads_mutex_);
  threads_.at(id)->DeactivateCallerThread();
}

bool ThreadManager::UnsetSignal(const api::ThreadId id,
                                api::ThreadSignal signal) noexcept {
  try {
    api::SharedLockGuard<api::SharedMutex> lock(threads_mutex_);
    threads_.at(id)->UnsetSignal(signal);
    return true;
  } catch (std::out_of_range) {
    return false;
  }
}
}  // namespace kernel
