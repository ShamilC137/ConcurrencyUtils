#include "API/DataStructures/Multithreading/UnboundedPriorityBlockingQueue.hpp"
#include <iostream>
#include <thread>
api::UnboundedPriorityBlockingQueue<int> queue;

void Init() {
  for (int index{100}; index >= 0; --index) {
    queue.Push(index);
    std::cout << "size: " << queue.Size() << '\n'; 
  }
  std::cout << "Done pushing\n";
}

void TryInit() {
  for (int index{50}; index > 0; --index) {
    std::cout << std::boolalpha << queue.TryEmplace(index) << '\n';
  }
}

void Pop() {
  for (int index{}; index <= 15; ++index) {
    std::cout << queue.Pop() << '\n';
  }
  std::cout << "Done poping\n";
}

void TryPop() {
  for (int index{}; index <= 15; ++index) {
    try {
      std::cout << queue.TryPop() << '\n';
    } catch (...) {
      std::cout << "Failed\n";
    }
  }
  std::cout << "Done poping\n";
}

int main() {
  std::thread thr4(TryInit);
  std::thread thr1(Init);
 // std::this_thread::sleep_for(std::chrono::seconds(1));
  std::thread thr2(TryPop);
  std::thread thr3(Pop);

  thr1.join();
  thr2.join();
  thr3.join();
  thr4.join();
  return 0;
}
