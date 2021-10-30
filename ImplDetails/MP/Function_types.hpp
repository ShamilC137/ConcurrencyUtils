#ifndef APPLICATION_IMPLDETAILS_MP_FUNCTION_TYPES_HPP_
#define APPLICATION_IMPLDETAILS_MP_FUNCTION_TYPES_HPP_
#include "MPVector.hpp"

namespace impl {
// Helper for lambdas or noncallable
template <class T> struct ComponentsHelper;

} // namespace impl

namespace api {
// Basic template, works if type is noncallable or functor
template <class Func> struct Components {
  using ReturnType = typename impl::ComponentsHelper<Func>::ReturnType;
  using ParametersTypes = typename impl::ComponentsHelper<Func>::ParametersTypes;
  using FunctorType = typename impl::ComponentsHelper<Func>::FunctorType;
};

// Function pointer specialization
template <class Ret, class... Args> struct Components<Ret (*)(Args...)> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
};

// Noexcept function pointer specialization
template <class Ret, class... Args>
struct Components<Ret (*)(Args...) noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
};

// Member-function pointer specialization
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...)> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
};

// Const member-function pointer specialization
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) const> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
};

// Volatile member-function pointer specialization
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) volatile> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
};

// Noexcept member-function pointer specialization
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
};

// Const noexcept member-function pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) const noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
};

// Volatile noexcept member-function pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) volatile noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
};

// Const volatile noexcept member-function pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) const volatile noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
};

// Lvalue reference member-function pointer specialization
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) &> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
};

// Const lvalue member-function pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) const &> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
};

// Const lvalue noexcept member-function pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) const &noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
};

// Const volatile lvalue noexcept member-function pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) const volatile &noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
};

// Rvalue reference member-function pointer specialization
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) &&> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
};

// Const rvalue member-function pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) const &&> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
};

// Const rvalue noexcept member-function pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) const &&noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
};

// Const volatile rvalue noexcept member-function pointer
template <class Ret, class Class, class... Args>
struct Components<Ret (Class::*)(Args...) const volatile &&noexcept> {
  using ReturnType = Ret;
  using ParametersTypes = MPVector<Args...>;
  using FunctorType = Class;
};
} // namespace api

namespace impl {
// Additional tools for implementation

// MSVC do not treat decltype(&ClassType::operator()) as dependent type
template <class T> constexpr typename api::Components<T> helper(T);

template <class...> using void_t = void;

// Checks that operator() exists
// Base template, assumes that operator() not existing
template <class T, class = void> constexpr static bool has_operator{false};

// Existance specialization
template <class T>
constexpr static bool has_operator<T, void_t<decltype(helper(&T::operator()))>>{
    true};

// Takes pointer to operator() and tries to decay it
template <class T> struct ComponentsHelper {
  static_assert(has_operator<T>, "Type must have functor or function type");
  using ReturnType = typename decltype(helper(&T::operator()))::ReturnType;
  using ParametersTypes =
      typename decltype(helper(&T::operator()))::ParametersTypes;
  using FunctorType = typename decltype(helper(&T::operator()))::FunctorType;
};
} // namespace impl
#endif // APPLICATION_IMPLDETAILS_MP_FUNCTION_TYPES_HPP_
