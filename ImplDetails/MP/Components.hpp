#ifndef APPLICATION_IMPLDETAILS_MP_FUNCTION_TYPES_HPP_
#define APPLICATION_IMPLDETAILS_MP_FUNCTION_TYPES_HPP_
#include "../Utility.hpp"
#include "MPVector.hpp"

/// <summary>
///   This file contains decomposer (Components) that decompose function type (
///   pure function type, pointer to function, class member function) on its
///   types with decomposer helpers.
///   Components class aliases:
///   1) ReturnType - function return type;
///   2) ParametersTypes - MPVector of function parameters types, mb empty.
///   3) FunctorType - if type is member-function type, represents its class
///   type
///   4) ellipsis - true if function have C-style ellipsis, otherwise false
/// </summary>
namespace impl {
namespace impl_details {
// Helper for lambdas or noncallable
template <class T>
struct ComponentsHelper;
}  // namespace impl_details
}  // namespace impl

namespace api {
// Basic template, works if type is noncallable or functor
template <class Func>
struct Components {
  using ReturnType =
      typename impl::impl_details::ComponentsHelper<Func>::ReturnType;
  using ParametersTypes =
      typename impl::impl_details::ComponentsHelper<Func>::ParametersTypes;
  using FunctorType =
      typename impl::impl_details::ComponentsHelper<Func>::FunctorType;
};

// Function specialization
template <class Ret, class... Args>
struct Components<Ret(Args...)> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  static constexpr bool ellipsis = false;
};

// Function with ellipsis specialization
template <class Ret, class... Args>
struct Components<Ret(Args......)> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  static constexpr bool ellipsis = true;
};

// Noexcept function specialization
template <class Ret, class... Args>
struct Components<Ret(Args...) noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  static constexpr bool ellipsis = false;
};

// Noexcept function with ellipsis specialization
template <class Ret, class... Args>
struct Components<Ret(Args......) noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  static constexpr bool ellipsis = true;
};

// Function pointer specialization
template <class Ret, class... Args>
struct Components<Ret (*)(Args...)> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  static constexpr bool ellipsis = false;
};

// Noexcept function pointer specialization
template <class Ret, class... Args>
struct Components<Ret (*)(Args...) noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  static constexpr bool ellipsis = false;
};

// Function pointer with ellipsis specialization
template <class Ret, class... Args>
struct Components<Ret (*)(Args......)> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  static constexpr bool ellipsis = true;
};

// Noexcept function pointer with ellipsis specialization
template <class Ret, class... Args>
struct Components<Ret (*)(Args......) noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  static constexpr bool ellipsis = true;
};

// Member-function pointer specialization
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...)> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = false;
};

// Member-function pointer with ellipsis specialization
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args......)> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = true;
};

// Const member-function pointer specialization
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) const> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = false;
};

// Const member-function pointer with ellipsis specialization
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args......) const> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = true;
};

// Volatile member-function pointer specialization
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) volatile> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = false;
};

// Volatile member-function pointer with ellipsis specialization
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args......) volatile> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = true;
};

// Noexcept member-function pointer specialization
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = false;
};

// Noexcept member-function pointer with ellipsis specialization
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args......) noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = true;
};

// Const noexcept member-function pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) const noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = false;
};

// Const noexcept member-function with ellipsis pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args......) const noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = true;
};

// Volatile noexcept member-function pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) volatile noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = false;
};

// Volatile noexcept member-function with ellipsis pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args......) volatile noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = true;
};

// Const volatile member-function specialization
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) const volatile> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = false;
};

// Const volatile member-function with ellipsis specialization
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args......) const volatile> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = true;
};

// Const volatile noexcept member-function pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) const volatile noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = false;
};

// Const volatile noexcept member-function with ellipsis pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args......) const volatile noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = true;
};

// Lvalue reference member-function pointer specialization
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) &> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = false;
};

// Lvalue reference member-function with ellipsis pointer specialization
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args......) &> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = true;
};

// Lvalue referenced noexcept member-function pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) &noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = false;
};

// Lvalue referenced noexcept member-function with ellipsis pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args......) &noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = true;
};

// Const lvalue member-function pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) const &> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = false;
};

// Const lvalue member-function with ellipsis pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args......) const &> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = true;
};

// Volatile lvalue member-function pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) volatile &> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = false;
};

// Volatile lvalue member-function with ellipsis pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args......) volatile &> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = true;
};

// Const lvalue noexcept member-function pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) const &noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = false;
};

// Const lvalue noexcept member-function with ellipsis pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args......) const &noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = true;
};

// Volatile lvalue noexcept member-function pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) volatile &noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = false;
};

// Volatile lvalue noexcept member-function with ellipsis pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args......) volatile &noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = true;
};

// Lvalue referenced const volatile member-function pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) const volatile &> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = false;
};

// Lvalue referenced const volatile member-function with ellipsis pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args......) const volatile &> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = true;
};

// Const volatile lvalue noexcept member-function pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) const volatile &noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = false;
};

// Const volatile lvalue noexcept member-function with ellipsis pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args......) const volatile &noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = true;
};

// Rvalue reference member-function pointer specialization
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) &&> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = false;
};

// Rvalue reference member-function with ellipsis pointer specialization
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args......) &&> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = true;
};

// Rvalue referenced noexcept member-function pointer specialization
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) &&noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = false;
};

// Rvalue referenced noexcept member-function with ellipsis pointer
// specialization
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args......) &&noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = true;
};

// Const rvalue member-function pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) const &&> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = false;
};

// Const rvalue member-function with ellipsis pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args......) const &&> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = true;
};

// Volatile rvalue member-function pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) volatile &&> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = false;
};

// Volatile rvalue member-function with ellipsis pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args......) volatile &&> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = true;
};

// Const rvalue noexcept member-function pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) const &&noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = false;
};

// Const rvalue noexcept member-function with ellipsis pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args......) const &&noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = true;
};

// Volatile rvalue noexcept member-function pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) volatile &&noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = false;
};

// Volatile rvalue noexcept member-function with ellipsis pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args......) volatile &&noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = true;
};

// Const rvalue noexcept member-function pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) const volatile &&> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = false;
};

// Const rvalue noexcept member-function with ellipsis pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args......) const volatile &&> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = true;
};

// Const volatile rvalue noexcept member-function pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) const volatile &&noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = false;
};

// Const volatile rvalue noexcept member-function with ellipsis pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args......) const volatile &&noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
  static constexpr bool ellipsis = true;
};
}  // namespace api

namespace impl {
namespace impl_details {
// Additional tools for implementation

// MSVC do not treat decltype(&ClassType::operator()) as dependent type
template <class T>
constexpr typename api::Components<T> helper(T);

// Checks that operator() exists
// Base template, assumes that operator() not existing
template <class T, class = void>
constexpr static bool has_operator{false};

// Existance specialization
template <class T>
constexpr static bool has_operator<T, VoidT<decltype(helper(&T::operator()))>>{
    true};

// Takes pointer to operator() and tries to decay it
template <class T>
struct ComponentsHelper {
  static_assert(has_operator<T>, "Type must have functor or function type");
  using ReturnType = typename decltype(helper(&T::operator()))::ReturnType;
  using ParametersTypes =
      typename decltype(helper(&T::operator()))::ParametersTypes;
  using FunctorType = typename decltype(helper(&T::operator()))::FunctorType;
};
}  // namespace impl_details
}  // namespace impl
#endif  // APPLICATION_IMPLDETAILS_MP_FUNCTION_TYPES_HPP_
