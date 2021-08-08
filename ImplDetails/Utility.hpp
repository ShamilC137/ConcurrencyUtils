#ifndef APPLICATION_IMPLDETAILS_UTILITY_HPP_
#define APPLICATION_IMPLDETAILS_UTILITY_HPP_

// STL
#include <exception>
#include <type_traits>

namespace impl {
template <class T> struct ForceExplicitType { using Type = T; };

template <class T>
using ForceExplicitTypeT = typename ForceExplicitType<T>::Type;

struct BaseTypeID {
  inline static int freeid{};
};

template <class T> struct TypeID : BaseTypeID {
  inline static int id{BaseTypeID::freeid++};
};

template <class... Args> struct IDSequence {
  static constexpr int (&CreateIDSequence() noexcept)[sizeof...(Args)] {
    if (!is_ids_createad) {
      CreateIDSHelper(std::make_index_sequence<sizeof...(Args)>{});
      is_ids_createad = true;
    }
    return ids;
  }

private:
  template <std::size_t... Indexes>
  static constexpr void
  CreateIDSHelper(std::index_sequence<Indexes...>) noexcept {
    ((ids[Indexes] = TypeID<Args>::id), ...);
  }

  static inline bool is_ids_createad;
  static inline int ids[sizeof...(Args)];
};
} // namespace impl

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
} // namespace api

#endif // !APPLICATION_IMPLDETAILS_UTILITY_HPP_
