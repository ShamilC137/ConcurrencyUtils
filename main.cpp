#include "ImplDetails/MP/Components.hpp"
#include "ImplDetails/Utility.hpp"
int main() {
  api::Components<int (*)()>::ParametersTypes;
  return 0;
}
/*#include <iostream>

#include "API/DataStructures/Multithreading/DeferThread.hpp"

volatile api::ThreadSignals sigs{};

volatile api::ThreadSignals &GetSignals() noexcept { return sigs; }

void Foo() { throw std::exception{}; }

void Terminate() { std::cout << "Terminated\n"; }

struct Str {
  void foo() {}
};

int main() {
  api::DeferThread thread(false, &Terminate, &Foo);
  thread.ActivateThread();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  sigs.Set(api::ThreadSignal::kSuspend);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  thread.ActivateThread();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  sigs.Set(api::ThreadSignal::kExit);
  thread.Join();
  return 0;
}*/
/*
#include <iostream>

#include "API/DataStructures/ScopedSlotWrapper.hpp"
#include "API/DataStructures/Slot.hpp"
#include "API/DataStructures/TaskWrapper.hpp"
#include "API/PublicAPI.hpp"
#include "Kernel/Kernel/Kernel.hpp"

api::Atomic<int> counter{};
static struct A {
  api::Atomic<int> &counter_;
  A() : counter_{counter} {}
  ~A() { std::cout << "counter: " << counter.load() << '\n'; }
} obj;
api::TaskWrapper tasks[20];

void Init() {
  for (int index{}; index < 5; ++index) {
    tasks[index] = {
        new api::Task<int>("sig1", api::TaskPriority::kHighPriotity, index),
        {}};
    tasks[index].GetTask()->SetNumOfAcceptors(2);
  }

  for (int index{5}; index < 10; ++index) {
    tasks[index] = {
        new api::Task<int>("sig", api::TaskPriority::kHighPriotity, index), {}};
    tasks[index].GetTask()->SetNumOfAcceptors(2);
  }
}

void Waiter() {
  for (int index{}; index < 10; ++index) {
    tasks[index].GetTask()->Wait();
    // std::cout << "Done waiting\n";
  }
}

void foo(int a1) {
  // std::cout << a1 << '\n';
  return;
}

void Caller1() {
  api::ScopedSlotWrapper slot_wrap(new api::Slot<void, int>(foo));
  slot_wrap.GetSlot()->SetPriority("sig", 1);
  slot_wrap.GetSlot()->SetPriority("sig1", -1);
  for (int index{}; index < 10; ++index) {
    try {
      (*slot_wrap.GetSlot())(tasks[index]);
    } catch (api::Deadlock &lc) {
      std::cout << lc.what() << '\n';
      std::terminate();
    }
  }
  // std::cout << "Slot done\n";
}

void Caller2() {
  api::ScopedSlotWrapper slot_wrap1(new api::Slot<void, int>(foo));
  slot_wrap1.GetSlot()->SetPriority("sig", 2);
  slot_wrap1.GetSlot()->SetPriority("sig1", -1);
  for (int index{}; index < 10; ++index) {
    try {
      (*slot_wrap1.GetSlot())(tasks[index]);
    } catch (api::Deadlock &lc) {
      std::cout << lc.what();
      std::terminate();
    }
  }
  // std::cout << "Slot1 done\n";
}

// sig_name is_blocking priority : slot_name slot_name1 slot_name2
int main() {
  for (int index{}; index < 1000; ++index) {
    api::TaskWrapper(
        api::Emit<void, int>("mod", false, api::TaskPriority::kLowPriority, 1));
    Init();
    std::thread thr3(Waiter);
    // std::this_thread::sleep_for(std::chrono::seconds(1));
    std::thread thr1(Caller1);
    std::thread thr2(Caller2);

    thr3.join();
    thr2.join();
    thr1.join();
  }

  return 0;
}
*/
