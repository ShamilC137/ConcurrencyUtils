#ifndef APPLICATION_IMPLDETAILS_MP_META_PROGRAMMING_VECTOR_HPP_
#define APPLICATION_IMPLDETAILS_MP_META_PROGRAMMING_VECTOR_HPP_

namespace impl {
template <class, class = void> constexpr static bool is_mpiterable{false};

template <class T>
constexpr static bool is_mpiterable<T, typename T::MPIterable>{true};

template <auto index, class MPIterable> struct Extractor {
  static_assert(is_mpiterable<MPIterable>, "Type must be MP iterable");
  static_assert(MPIterable::size > index, "Index out of range");
  using Type = typename Extractor<index - 1, typename MPIterable::Base>::Type;
};

template <class MPIterable> struct Extractor<0, MPIterable> {
  using Type = typename MPIterable::Type;
};
} // namespace impl

namespace api {
template <class Head = void, class... Args> struct MPVector : MPVector<Args...> {
  using Type = Head;
  using Base = MPVector<Args...>;
  using MPIterable = void;
  constexpr static unsigned size{sizeof...(Args) + 1u};
};

template <class Head> struct MPVector<Head> {
  using Type = Head;
  using MPIterable = void;
  constexpr static unsigned size{1u};
};

template <> struct MPVector<> { constexpr static unsigned size{0u}; };

template <auto index, class MPIterable>
using TypeExtractor = typename impl::Extractor<index, MPIterable>::Type;
} // namespace api

#endif // APPLICATION_IMPLDETAILS_MP_META_PROGRAMMING_VECTOR_HPP_
