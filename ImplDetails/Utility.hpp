#ifndef APPLICATION_IMPLDETAILS_UTILITY_HPP_
#define APPLICATION_IMPLDETAILS_UTILITY_HPP_

// STL
#include <stdexcept>
#include <type_traits>

namespace impl {
template <class, class>
constexpr static bool is_same{false};

template <class T>
constexpr static bool is_same<T, T>{true};

template <bool... args>
constexpr static bool conjunction = (args && ...);

template <class T, class... Args>
constexpr static bool is_all_same = conjunction<is_same<T, Args>...>;

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
struct IdSequence {
  static constexpr int (&CreateIdSequence() noexcept)[sizeof...(Args)] {
    if (!is_ids_createad) {
      CreateIdsHelper(std::make_index_sequence<sizeof...(Args)>{});
      is_ids_createad = true;
    }
    return ids;
  }

 private:
  template <std::size_t... Indexes>
  static constexpr void CreateIdsHelper(
      std::index_sequence<Indexes...>) noexcept {
    ((ids[Indexes] = TypeID<Args>::id), ...);
  }

  static inline bool is_ids_createad;
  static inline int ids[sizeof...(Args)];
};

template <class...>
using VoidT = void;
}  // namespace impl

namespace api {
struct BadSlotCall : std::runtime_error {
  using Base = std::runtime_error;
  using Base::Base;
  using Base::what;
};

struct BrokenReturnTask : std::logic_error {
  using Base = std::logic_error;
  using Base::Base;
  using Base::what;
};

struct Deadlock : std::logic_error {
  using Base = std::logic_error;
  using Base::Base;
  using Base::what;
};

enum class OperationResult { kNone, kFail, kSuccess };
}  // namespace api

#endif  // !APPLICATION_IMPLDETAILS_UTILITY_HPP_
