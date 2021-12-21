#include "TaskDetails.hpp"

namespace impl {
// ctor and dtor
BaseTask::BaseTask(const api::String &signal_sig, api::TaskPriority priority,
                   const int *idseq, const int *retid) noexcept
    : caused_signal_sig_{signal_sig},
      priority_{priority},
      idseq_ptr_{idseq},
      retid_ptr_{retid},
      nreferences_{},
      nacceptors_{} {}

BaseTask::~BaseTask() noexcept {
  assert(nreferences_.load(api::MemoryOrder::relaxed) == 0u &&
         "Deleting alive task");
}

void BaseTask::SetCausedSignal(const api::String &signal) {
  api::ScopedLock<api::SharedMutex> lock(signal_mutex_);
  caused_signal_sig_ = signal;
}

inline const api::String &BaseTask::GetCausedSignal() const noexcept {
  api::SharedLockGuard<api::SharedMutex> lock(signal_mutex_);
  return caused_signal_sig_;
}

// modifiers
void BaseTask::NotifyAboutComplete() noexcept {
  nacceptors_.sub(1u, api::MemoryOrder::release);
  if (!IsWaitable()) {
    ClearArguments();
  }
  nacceptors_.notify_all();
}

// sync operations
void BaseTask::Wait(const unsigned char expected_value) noexcept(false) {
  auto old{nacceptors_.load(api::MemoryOrder::relaxed)};
  if (expected_value > old) {
    throw api::Deadlock("Unreachible expected value");
  }

  for (; old != expected_value;
       old = nacceptors_.load(api::MemoryOrder::relaxed)) {
    nacceptors_.wait(old, api::MemoryOrder::relaxed);
  }
}
}  // namespace impl
