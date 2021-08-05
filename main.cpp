#include "API/DataStructures/Slot.hpp"
#include <chrono>
#include <functional>
#include <iostream>
#include <thread>

int zoo(int a1) {
  // std::cout << a1 << '\n';
  return a1;
};

impl::BaseTask *tasks[10];

void foo() {
  std::this_thread::sleep_for(std::chrono::seconds(1));
  for (int index{}; index < 10; ++index) {
    impl::BaseSlot *slot(new api::Slot<int, int>(zoo));
    slot->SetPriority(1);
    try {
      (*slot)(tasks[index]);
      std::cout
          << static_cast<api::ReturnTask<int, int> *>(tasks[index])->GetResult()
          << '\n';
    } catch (std::exception &ex) {
      std::cout << ex.what() << '\n';
    }
  }
}

void foo1() {
//  std::this_thread::sleep_for(std::chrono::seconds(1));
  for (int index{}; index < 10; ++index) {
    impl::BaseSlot *slot(new api::Slot<int, int>(zoo));
    //slot->SetPriority(1);
    try {
      api::TaskWrapper task(tasks[index]);
      (*slot)(task);
      std::cout
          << static_cast<api::ReturnTask<int, int> *>(tasks[index])->GetResult()
          << '\n';
    } catch (std::exception &ex) {
      std::cout << ex.what() << '\n';
    }
    delete slot;
  }
}

void boo() {
  for (int index{}; index < 10; ++index) {
    auto task = new api::ReturnTask<int, int>("mod", index); // BaseTask *
    task->SetNumOfAcceptors(1);
    tasks[index] = task;
  }
}

void Delete() {
  for (int index{}; index < 10; ++index) {
    try {
      delete tasks[index];
    } catch (std::exception &ex) {
      std::cout << ex.what() << '\n';
    }
  }
}

#include "API/PublicAPI.hpp"
int main() {
  //boo();
  //std::thread thread1(Delete);
 // std::this_thread::sleep_for(std::chrono::seconds(1));
  //std::thread thread2(foo);
  //std::thread thread3(foo1);

  //thread1.join();
  //thread2.join();
  //thread3.join();

  api::Emit<int, int>("mid", false, 1);
  return 0;
}
