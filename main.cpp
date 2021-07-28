#include "API/DataStructures/Multithreading/UnboundedBlockingQueue.hpp"
#include <chrono>
#include <functional>
#include <iostream>
#include <thread>
using Task = std::function<void()>;
api::UnboundedPriorityBlockingQueue<Task> cont;

void foo() {
  while (true) {
    auto task{cont.Pop()};
    if (!task) {
      return;
    }
    task();
  }
}

int var;
void boo() {
  std::this_thread::sleep_for(std::chrono::seconds(5)); // pop waiting check
  for (int index{}; index < 10; ++index) {
    cont.Push([]() { std::cout << ++var << '\n'; });
  }
  cont.Push(nullptr);
}

bool operator==(const Task &, const Task &) { return true; }
bool operator!=(const Task &, const Task &) { return false; }
bool operator>(const Task &, const Task &) { return false; }
bool operator<(const Task &, const Task &) { return false; }
bool operator>=(const Task &, const Task &) { return true; }
bool operator<=(const Task &, const Task &) { return true; }

int main() {
  std::thread t1(foo);
  std::thread t2(boo);
  t1.join();
  t2.join();
  return 0;
}
