#ifndef APPLICATION_API_DATASTRUCTURES_MULTITHREADING_THREADSIGNALS_HPP_
#define APPLICATION_API_DATASTRUCTURES_MULTITHREADING_THREADSIGNALS_HPP_
// STL
#include <cassert>

// current project
#include "../../../ImplDetails/Utility.hpp"

namespace api {
using ThreadSignalUnderlyingType = unsigned char;

enum class ThreadSignal : ThreadSignalUnderlyingType {
  kEmpty = 0b0,
  kExit = 0b10,
  kSuspend = 0b100,
  kResume = 0x80,
};

class ThreadSignals {
  using Type = ThreadSignalUnderlyingType;

  // ctors
 public:
  inline ThreadSignals() noexcept : value_{} {}
  template <class... Signals>
  ThreadSignals(Signals... sigs) noexcept : value_{} {
    static_assert(impl::IsAllSame<ThreadSignal, Signals...>::value,
                  "Constructor takes only ThreadSignal");
    ((Set(sigs)), ...);
  }

  ThreadSignals(const ThreadSignals &rhs) noexcept;

  ThreadSignals(ThreadSignals &&rhs) noexcept;

  // Operators =
 public:
  volatile ThreadSignals &operator=(const ThreadSignals &rhs) volatile noexcept;

  volatile ThreadSignals &operator=(ThreadSignals &&) volatile noexcept;

 private:
  // for operator &
  ThreadSignals(Type value);

  // Test and set
 public:
  [[nodiscard]] bool Test(ThreadSignal sig) const volatile noexcept;

  void Set(ThreadSignal sig) volatile noexcept;

  void Unset(ThreadSignal sig) volatile noexcept;

  // cast
 public:
  // If there are more than one value, cast returns ThreadSignal with the lowest
  // value
  operator ThreadSignal() const volatile noexcept;

  // friends
 private:
  friend ThreadSignals operator&(const volatile ThreadSignals &lhs,
                                 const volatile ThreadSignals &rhs) noexcept;

  friend ThreadSignals operator~(const volatile ThreadSignals &rhs) noexcept;

  friend bool operator==(const volatile ThreadSignals &lhs,
                         const volatile ThreadSignals &rhs) noexcept;

  friend bool operator!=(const volatile ThreadSignals &lhs,
                         const volatile ThreadSignals &rhs) noexcept;

 private:
  volatile Type value_;
};

}  // namespace api

#endif  // APPLICATION_API_DATASTRUCTURES_MULTITHREADING_THREADSIGNALS_HPP_
