#ifndef APPLICATION_API_DATASTRUCTURES_MULTITHREADGING_DEFERTASKWRAPPER
#define APPLICATION_API_DATASTRUCTURES_MULTITHREADGING_DEFERTASKWRAPPER

// This class merely wraps the DeferThread
// Motivation: hide implementation details of the DeferThread (such atomic
// activation) from user.

// current project
#include "DeferThread.hpp"

namespace api {
class DeferThreadWrapper {
public:
  DeferThreadWrapper() noexcept;

  DeferThreadWrapper(DeferThread *thread) noexcept;

  DeferThreadWrapper(const DeferThreadWrapper &rhs) noexcept;

  DeferThreadWrapper(DeferThreadWrapper &&rhs) noexcept;
  
  DeferThreadWrapper &operator=(const DeferThreadWrapper &rhs) noexcept;

  DeferThreadWrapper &operator=(DeferThreadWrapper &&rhs) noexcept;

  void Detach();

  void Join();

  auto NativeHandle();

  [[nodiscard]] bool Joinable() const noexcept;

  [[nodiscard]] unsigned int HardwareConcurrency() noexcept;

  [[nodiscard]] auto GetId() const noexcept;

private:
  DeferThread *thread_; // pointer to associated thread
};
} // namespace api

#endif // APPLICATION_API_DATASTRUCTURES_MULTITHREADGING_DEFERTASKWRAPPER
