#ifndef APPLICATION_API_DATASTRUCTURES_TASK_HPP_
#define APPLICATION_API_DATASTRUCTURES_TASK_HPP_

// current project
#include "Containers/String.hpp"
#include "../../ImplDetails/TaskDetails.hpp"

// STL
#include <tuple>

namespace api {
template <class... Args>
class Task : public impl::BaseTask {
 public:
  using Base = impl::BaseTask;
  using Tuple = std::tuple<Args...>;

 public:
  Task(typename impl::ForceExplicitTypeT<Args>... args)
      : Base(impl::IDSequence<Args...>::CreateIDSequence()),
        data_{std::forward<Args>(args)...} {}

 private:
  template <class T, std::size_t... Seq>
  void UnpackHelper(T &where, std::index_sequence<Seq...>) {
    (where.push_back(std::forward<Args>(std::get<Seq>(data_))), ...);
  }

  template <class T, std::size_t... Seq>
  void UnpackHelper(T &where, std::index_sequence<Seq...>) const {
    (where.push_back(
         std::forward<std::add_const_t<Args>>(std::get<Seq>(data_))),
     ...);
  }

 public:
  template <class Container>
  void Unpack(Container &where) {
    constexpr auto size{std::tuple_size_v<Tuple>};
    UnpackHelper(where, std::make_index_sequence<size>{});
  }

  template <class T>
  void Unpack(T &where) const {
    constexpr auto size{std::tuple_size_v<Tuple>};
    UnpackHelper(where, std::make_index_sequence<size>{});
  }

  [[nodiscard]] inline Tuple &GetTuple() noexcept { return data_; }

  [[nodiscard]] inline const Tuple &GetTuple() const noexcept { return data_; }

  // fields
 private:
  Tuple data_;
};

}

#endif // !APPLICATION_API_DATASTRUCTURES_TASK_HPP_
