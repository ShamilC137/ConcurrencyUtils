#include "API/DataStructures/Multithreading/UnboundedPriorityBlockingQueue.hpp"
#include <chrono>
#include <functional>
#include <iostream>
#include <thread>
#include "API/DataStructures/Slot.hpp"

using Task = api::Task<int> *;
api::UnboundedPriorityBlockingQueue<Task> cont;

int zoo(int a1) {
  return a1;
};

void foo() {

  while (true) {
    impl::BaseSlot *slot(new api::Slot<int, int>(zoo));
    cont.Size();
    cont.Empty();
    auto task{cont.Pop()};
    if (!task) {
      return;
    }
    (*slot)(task);
    std::cout << static_cast<api::ReturnTask<int, int> *>(task)->GetResult()
              << '\n';
    delete task;
  }
}
void boo() {
  for (int index{}; index < 10; ++index) {
    auto task = new api::ReturnTask<int, int>("mod", index); // BaseTask *
    task->SetNumOfAcceptors(1);
    cont.Push(task);
  }
  cont.Push(nullptr);
}

int main() {
  std::thread thread1(foo);
  std::thread thread2(boo);
  thread1.join();
  thread2.join();
  return 0;
}
