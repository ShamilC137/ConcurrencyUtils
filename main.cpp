#include "API/DataStructures/TaskWrapper.hpp"
#include "API/DataStructures/ScopedSlotWrapper.hpp"
#include "API/DataStructures/Slot.hpp"
#include "API/PublicAPI.hpp"
#include <iostream>
#include <thread>

api::TaskWrapper tasks[20];

void Init() {
  for (int index{}; index < 5; ++index) {
    tasks[index] = {
        new api::Task<int>("mod", false, api::TaskPriority::kHighPriotity, index)};
    tasks[index].GetTask()->SetNumOfAcceptors(1);
  }
}

void Waiter() {
  for (int index{}; index < 5; ++index) {
    tasks[index].GetTask()->Wait();
    std::cout << "Done waiting\n";
 }
}

void foo(int a1) {
  std::cout << a1 << '\n';
  return;
}

void Caller() {
  api::ScopedSlotWrapper slot_wrap(new api::Slot<void, int>(foo));
  for (int index{}; index < 5; ++index) {
    (*slot_wrap.GetSlot())(tasks[index]);
    std::cout << "Slot work done\n";
  }
}

// sig_name is_blocking priority : slot_name slot_name1 slot_name2

int main() { 
  api::Emit<void, int>("mod", false, api::TaskPriority::kLowPriority, 1);
  Init();
  std::thread thr3(Waiter);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::thread thr2(Caller);


  thr3.join();
  thr2.join();
  return 0; 
}
