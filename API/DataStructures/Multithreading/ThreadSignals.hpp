#ifndef APPLICATION_API_DATASTRUCTURES_MULTITHREADING_THREADSIGNALS_HPP_
#define APPLICATION_API_DATASTRUCTURES_MULTITHREADING_THREADSIGNALS_HPP_
#include <cassert>

namespace impl {
template <class, class> constexpr static bool is_same{false};
template <class T> constexpr static bool is_same<T, T>{true};
template <class First, class Second = First, class... Types> struct IsAllSame {
  constexpr static bool value =
      is_same<First, Second> & IsAllSame<Second, Types...>::value;
};

template <class First, class Second> struct IsAllSame<First, Second> {
  constexpr static bool value = is_same<First, Second>;
};
} // namespace impl

namespace api {
using ThreadSignalUnderlyingType = unsigned char;

enum class ThreadSignal : ThreadSignalUnderlyingType {
  kEmpty = 0b0,
  kExitAfterCall = 0b1,
  kExit = 0b10,
  kSuspend = 0b100
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
  operator ThreadSignal() const noexcept;

  // friends
private:

  friend ThreadSignals operator&(const volatile ThreadSignals &lhs,
                                 const volatile ThreadSignals &rhs) noexcept;

  friend bool operator==(const volatile ThreadSignals &lhs,
                         const volatile ThreadSignals &rhs) noexcept;

  friend bool operator!=(const volatile ThreadSignals &lhs,
                         const volatile ThreadSignals &rhs) noexcept;

private:
  Type value_;
};

} // namespace api

#endif // APPLICATION_API_DATASTRUCTURES_MULTITHREADING_THREADSIGNALS_HPP_
