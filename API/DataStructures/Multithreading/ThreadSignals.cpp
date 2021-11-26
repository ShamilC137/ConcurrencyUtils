#include "ThreadSignals.hpp"

namespace api {
// ctors

ThreadSignals::ThreadSignals(const ThreadSignals &rhs) noexcept
    : value_{rhs.value_} {}

ThreadSignals::ThreadSignals(ThreadSignals &&rhs) noexcept
    : value_{rhs.value_} {
  rhs.value_ = static_cast<Type>(ThreadSignal::kExit);
}

// private ctor
ThreadSignals::ThreadSignals(Type value) : value_{value} {}

// operators =

ThreadSignals &ThreadSignals::operator=(const ThreadSignals &rhs) noexcept {
  value_ = rhs.value_;
  return *this;
}

ThreadSignals &ThreadSignals::operator=(ThreadSignals &&rhs) noexcept {
  value_ = rhs.value_;
  rhs.value_ = static_cast<Type>(ThreadSignal::kExit);
  return *this;
}

// test and set

[[nodiscard]] bool ThreadSignals::Test(ThreadSignal sig) const noexcept {
  return value_ & static_cast<Type>(sig);
}

void ThreadSignals::Set(ThreadSignal sig) noexcept {
  value_ |= static_cast<Type>(sig);
}

void ThreadSignals::Unset(ThreadSignal sig) noexcept {
  value_ &= ~static_cast<Type>(sig);
}

// cast operator
ThreadSignals::operator ThreadSignal() const noexcept {
  // kEmpty signal is not considered if any signal flag is enabled
  if (value_ & (value_ - 1)) {
    const auto kTypeSize{static_cast<Type>(sizeof(Type) * 8u)};
    for (Type index{}; index < kTypeSize; ++index) {
      const auto current{static_cast<Type>((value_ & (1u << index)))};
      if (current) {
        return static_cast<ThreadSignal>(current);
      }
    }
  } else {
    return static_cast<ThreadSignal>(value_);
  }
  return ThreadSignal::kExit;
}

// friends

ThreadSignals operator&(const ThreadSignals &lhs,
                        const ThreadSignals &rhs) noexcept {
  return static_cast<ThreadSignals::Type>(lhs.value_ & rhs.value_);
}

ThreadSignals operator~(const ThreadSignals &rhs) noexcept {
  return static_cast<ThreadSignals::Type>(~rhs.value_);
}

[[nodiscard]] bool operator==(const ThreadSignals &lhs,
                              const ThreadSignals &rhs) noexcept {
  return lhs.value_ == rhs.value_;
}

[[nodiscard]] bool operator!=(const ThreadSignals &lhs,
                              const ThreadSignals &rhs) noexcept {
  return !(lhs == rhs);
}

}  // namespace api
