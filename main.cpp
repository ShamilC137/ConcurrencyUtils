#include "API/DataStructures/Multithreading/UnboundedPriorityBlockingQueue.hpp"
#include "API/DataStructures/Slot.hpp"
#include <chrono>
#include <functional>
#include <iostream>
#include <thread>


using Task = std::function<void()>;
api::UnboundedPriorityBlockingQueue<Task> cont;

void foo() {
  while (true) {

    cont.Size();
    cont.Empty();
    std::this_thread::sleep_for(std::chrono::seconds(1)); // pop waiting check
    auto task{cont.Pop()};
    if (!task) {
      return;
    }
    task();
  }
}

int var;
void boo() {
  std::this_thread::sleep_for(std::chrono::seconds(3)); // pop waiting check
  for (int index{}; index < 10; ++index) {
    std::this_thread::sleep_for(std::chrono::seconds(3)); // pop waiting check
    cont.Push([]() { std::cout << ++var << '\n'; });
  }
  std::this_thread::sleep_for(std::chrono::seconds(5)); // pop waiting check
  cont.Push(nullptr);
}

bool operator==(const Task &, const Task &) { return true; }
bool operator!=(const Task &, const Task &) { return false; }
bool operator>(const Task &, const Task &) { return false; }
bool operator<(const Task &, const Task &) { return false; }
bool operator>=(const Task &, const Task &) { return true; }
bool operator<=(const Task &, const Task &) { return true; }

int boo(int a1, const int &a2, int &&a3, double a4) {
  std::cout << "a1: " << a1 << '\n';
  std::cout << "a2: " << a2 << '\n';
  std::cout << "a3: " << a3 << '\n';
  std::cout << "a4: " << a4 << '\n';
  return {};
}

int main() { 
  
  return 0;
}
