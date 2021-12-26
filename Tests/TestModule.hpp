#ifndef APPLICATION__TESTS_TESTMODULE_HPP_
#define APPLICATION__TESTS_TESTMODULE_HPP_

#include <exception>
#include <iostream>

#include "../API/DataStructures/Containers/String.hpp"
#include "../API/DataStructures/Slot.hpp"
#include "../API/PublicAPI.hpp"
#include "../ImplDetails/AbstractModule.hpp"

namespace test {

class TestModule : public impl::AbstractModule {
  using Base = impl::AbstractModule;

 public:
  TestModule() : Base("test_module"), str_{"some string"} {}

  ~TestModule() noexcept override;

  // essential
 public:
  impl::ModuleInitErrorStatus Init() noexcept override;

  // slots
 public:
  void HelloSlot();

  int SomeActionSlot();

  void ParameterizedSlot(int, double);

  const api::String &ReturnRefParameterizedSlot(int);

 private:
  api::String str_;
};

void TestRoutine(TestModule *module);

void TerminateHandler(std::exception ex);
}  // namespace test
#endif  // APPLICATION__TESTS_TESTMODULE_HPP_
