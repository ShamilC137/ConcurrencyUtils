#include "API/DataStructures/Slot.hpp"
#include <chrono>
#include <functional>
#include <iostream>
#include <thread>

int zoo(int a1) {
  std::cout << a1 << '\n';
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
    } catch (std::exception &ex) {
      std::cout << ex.what() << '\n';
    }
  }
}

void foo1() {
  std::this_thread::sleep_for(std::chrono::seconds(1));
  for (int index{}; index < 10; ++index) {
    impl::BaseSlot *slot(new api::Slot<int, int>(zoo));
    slot->SetPriority(2);
    try {
      (*slot)(tasks[index]);
    } catch (std::exception &ex) {
      std::cout << ex.what() << '\n';
    }
  }
}

void boo() {
  for (int index{}; index < 10; ++index) {
    auto task = new api::Task<int>("mod", false, index); // BaseTask *
    task->SetNumOfAcceptors(2);
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

int main() {
  boo();
  std::thread thread2(Delete);
  std::thread thread1(foo);
  std::thread thread3(foo1);
  thread2.join();
  thread3.join();
  thread1.join();

  return 0;
}
