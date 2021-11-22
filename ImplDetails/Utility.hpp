#ifndef APPLICATION_IMPLDETAILS_UTILITY_HPP_
#define APPLICATION_IMPLDETAILS_UTILITY_HPP_

// STL
#include <exception>
#include <type_traits>

namespace impl {
template <class, class>
constexpr static bool is_same{false};

template <class T>
constexpr static bool is_same<T, T>{true};

template <class First, class Second = First, class... Types>
struct IsAllSame {
  constexpr static bool value =
      is_same<First, Second> & IsAllSame<Second, Types...>::value;
};

template <class First, class Second>
struct IsAllSame<First, Second> {
  constexpr static bool value = is_same<First, Second>;
};

template <class T>
struct ForceExplicitType {
  using Type = T;
};

template <class T>
using ForceExplicitTypeT = typename ForceExplicitType<T>::Type;

struct BaseTypeID {
  inline static int freeid{};
};

template <class T>
struct TypeID : BaseTypeID {
  inline static int id{BaseTypeID::freeid++};
};

template <class... Args>
struct IDSequence {
  static constexpr int (&CreateIDSequence() noexcept)[sizeof...(Args)] {
    if (!is_ids_createad) {
      CreateIDSHelper(std::make_index_sequence<sizeof...(Args)>{});
      is_ids_createad = true;
    }
    return ids;
  }

 private:
  template <std::size_t... Indexes>
  static constexpr void CreateIDSHelper(
      std::index_sequence<Indexes...>) noexcept {
    ((ids[Indexes] = TypeID<Args>::id), ...);
  }

  static inline bool is_ids_createad;
  static inline int ids[sizeof...(Args)];
};

template <class...>
using VoidT = void;

// Checks that operator() exists
// Base template, assumes that operator() not existing
template <class T, class = void>
constexpr static bool HasOperator{false};

// Existance specialization
template <class T>
constexpr static bool HasOperator<T, VoidT<decltype(helper(&T::operator()))>>{
    true};
}  // namespace impl

namespace api {
struct BadSlotCall : std::exception {
  using Base = std::exception;
  using Base::Base;
  using Base::what;
};

struct BrokenReturnTask : std::exception {
  using Base = std::exception;
  using Base::Base;
  using Base::what;
};

struct Deadlock : std::exception {
  using Base = std::exception;
  using Base::Base;
  using Base::what;
};

enum class OperationResult { kNone, kFail, kSuccess };
}  // namespace api

#endif  // !APPLICATION_IMPLDETAILS_UTILITY_HPP_
