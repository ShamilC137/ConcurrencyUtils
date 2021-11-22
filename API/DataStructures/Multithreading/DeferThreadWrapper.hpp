#ifndef APPLICATION_API_DATASTRUCTURES_MULTITHREADGING_DEFERTASKWRAPPER
#define APPLICATION_API_DATASTRUCTURES_MULTITHREADGING_DEFERTASKWRAPPER

// This class merely wraps the DeferThread
// Motivation: hide implementation details of the DeferThread (such atomic
// activation) from user.

// current project
#include "DeferThread.hpp"

// STD
#include <exception>

namespace api {
struct ExpiredThread : std::exception {
  using MyBase = std::exception;
  using MyBase::exception;
  using MyBase::operator=;
  using MyBase::what;
  inline ~ExpiredThread() {}
};
}  // namespace api

namespace api {
class DeferThreadWrapper {
 public:
  DeferThreadWrapper() noexcept;

  ~DeferThreadWrapper() noexcept;

  DeferThreadWrapper(api::DeferThread *thread) noexcept;

  DeferThreadWrapper(const DeferThreadWrapper &rhs) noexcept;

  DeferThreadWrapper(DeferThreadWrapper &&rhs) noexcept;

  DeferThreadWrapper &operator=(const DeferThreadWrapper &rhs) noexcept;

  DeferThreadWrapper &operator=(DeferThreadWrapper &&rhs) noexcept;

  void Detach();

  void Join();

  auto NativeHandle() noexcept(false);

  [[nodiscard]] bool Joinable() noexcept(false);

  [[nodiscard]] unsigned int HardwareConcurrency() noexcept(false);

  [[nodiscard]] auto GetId() noexcept(false);

  void SendSuspendRequest() noexcept;

  bool Resume() noexcept(false);

  [[nodiscard]] bool IsClosed() const noexcept;

 private:
  DeferThread *thread_;  // pointer to associated thread
};

[[nodiscard]] ThreadId GetId(DeferThreadWrapper &thread) noexcept;
}  // namespace api

#endif  // APPLICATION_API_DATASTRUCTURES_MULTITHREADGING_DEFERTASKWRAPPER
