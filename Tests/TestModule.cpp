#include "TestModule.hpp"

namespace test {
TestModule::~TestModule() noexcept {}

// essentials

impl::ModuleInitErrorStatus TestModule::Init() noexcept {
  ForceConnections();
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
  Base::SetSlotsPriorities();  // FIXME:
  Base::slots_["void HelloSlot()"].get()->SetPriority("void Hello()", -1);

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

void TestModule::ForceConnections() {
  auto& mng{api::kernel_api::GetKernel().task_manager_};
  mng.connections_signatures_["void Hello()"] =
      api::Vector<api::Pair<api::String, kernel::ModuleDescriptor*>>{
          {"void HelloSlot()", mng.modules_[0]}};
}

void TestRoutine(TestModule* module) {
  try {
    decltype(auto) task{module->TryExtractTask()};
    module->ExecuteTask(task);
  } catch (api::PopFailed& ex) {
    std::cout << "Logged: " << ex.what() << '\n';
  }
}

void TerminateHandler(std::exception ex) {
  std::cout << "Unexcepted exception logged " << ex.what() << '\n';
}

}  // namespace test
