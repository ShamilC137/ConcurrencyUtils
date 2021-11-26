#ifndef APPLICATION_API_DATASTRUCTURES_MULTITHREADING_THREADSIGNALS_HPP_
#define APPLICATION_API_DATASTRUCTURES_MULTITHREADING_THREADSIGNALS_HPP_
// STL
#include <cassert>

// current project
#include "../../../Config.hpp"
#include "../../../ImplDetails/Utility.hpp"

namespace api {
using ThreadSignalUnderlyingType = unsigned char;

enum class ThreadSignal : ThreadSignalUnderlyingType {
  kEmpty = 0b0,
  kExit = 0b10,
  kSuspend = 0b100,
#if IS_DEBUG
  kResume = 0x80  // used to find deadlocks
#endif
};

class ThreadSignals {
 public:
  using Type = ThreadSignalUnderlyingType;

  // ctors
 public:
  inline ThreadSignals() noexcept : value_{} {}
  template <class... Signals>
  ThreadSignals(Signals... sigs) noexcept : value_{} {
    static_assert(impl::is_all_same<ThreadSignal, Signals...>::value,
                  "Constructor takes only ThreadSignal");
    ((Set(sigs)), ...);
  }

  ThreadSignals(const ThreadSignals &rhs) noexcept;

  ThreadSignals(ThreadSignals &&rhs) noexcept;

  // Operators =
 public:
  ThreadSignals &operator=(const ThreadSignals &rhs) noexcept;

  ThreadSignals &operator=(ThreadSignals &&) noexcept;

 private:
  // for operator &
  ThreadSignals(Type value);

  // Test and set
 public:
  [[nodiscard]] bool Test(ThreadSignal sig) const noexcept;

  void Set(ThreadSignal sig) noexcept;

  void Unset(ThreadSignal sig) noexcept;

  // cast
 public:
  // If there are more than one value, cast returns ThreadSignal with the lowest
  // value
  operator ThreadSignal() const noexcept;

  // friends
 private:
  friend ThreadSignals operator&(const ThreadSignals &lhs,
                                 const ThreadSignals &rhs) noexcept;

  friend ThreadSignals operator~(const ThreadSignals &rhs) noexcept;

  friend bool operator==(const ThreadSignals &lhs,
                         const ThreadSignals &rhs) noexcept;

  friend bool operator!=(const ThreadSignals &lhs,
                         const ThreadSignals &rhs) noexcept;

  friend class DeferThread;

 private:
  Type value_;
};

}  // namespace api

#endif  // APPLICATION_API_DATASTRUCTURES_MULTITHREADING_THREADSIGNALS_HPP_
