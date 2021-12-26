#include "TestModule.hpp"

namespace test {
TestModule::~TestModule() noexcept {}

// essentials

impl::ModuleInitErrorStatus TestModule::Init() noexcept {
  api::CreateThread(false, &TerminateHandler, &TestRoutine,
                    this);  // main routine
  AddSlotToTable("void HelloSlot()",
                 new api::Slot<void>(this, &TestModule::HelloSlot));
  AddSlotToTable("int SomeActionSlot()",
                 new api::Slot<int>(this, &TestModule::SomeActionSlot));
  AddSlotToTable(
      "void ParameterizedSlot(int, double)",
      new api::Slot<void, int, double>(this, &TestModule::ParameterizedSlot));
  AddSlotToTable("void RefParameterizedSlot(int)",
                 new api::Slot<const api::String&, int>(
                     this, &TestModule::ReturnRefParameterizedSlot));
  Base::SetSlotsPriorities();

  return impl::ModuleInitErrorStatus::kOk;
}

// slots
void TestModule::HelloSlot() { std::cout << "Hello, model!\n"; }

int TestModule::SomeActionSlot() { return 123; }

void TestModule::ParameterizedSlot(int first, double second) {
  std::cout << first << '\n';
  std::cout << second << '\n';
}

const api::String& TestModule::ReturnRefParameterizedSlot(int param) {
  std::cout << param << '\n';
  return str_;
}

void TestRoutine(TestModule* module) {
  std::cout << "Hi!\n";
  try {
    decltype(auto) task{module->TryExtractTask()};
    module->ExecuteTask(task);
  } catch (api::PopFailed& ex) {
    std::cout << "Logged\n";
  }
}

void TerminateHandler(std::exception ex) {
  std::cout << "Unexcepted exception logged\n";
}

}  // namespace test
