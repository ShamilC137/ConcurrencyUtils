#ifndef APPLICATION_IMPLDETAILS_THREADDESCRIPTOR_HPP_
#define APPLICATION_IMPLDETAILS_THREADDESCRIPTOR_HPP_
#include "../API/DataStructures/Multithreading/Atomics.hpp"
#include "../API/DataStructures/Multithreading/DeferThread.hpp"
#include "../API/DataStructures/Multithreading/Mutex.hpp"
#include "../API/DataStructures/Multithreading/ScopedLock.hpp"
namespace impl {
class ThreadDescriptor {
public:
  ThreadDescriptor(api::DeferThread *thread) noexcept;

  ThreadDescriptor(const ThreadDescriptor &) = delete;

  ThreadDescriptor &operator=(const ThreadDescriptor &) = delete;

  // Decrements number of DeferThreadWrapper objects, associated with this
  // object
  void DecrementNumberOfReferences() noexcept;

  // Increments number of DeferThreadWrapper objects, associated with this
  // object. If return result is false, DeferThreadWrapper objects can not be
  // created.
  bool IncrementNumberOfReferences() noexcept;

  [[nodiscard]] api::DeferThread *GetThread() noexcept;

  // Returns thread state. If true - associated thread could be deleted when
  // CouldBeDeleted returns true and thread_ is not Joinable
  [[nodiscard]] const volatile bool &ShallBeDeleted() const noexcept;

  // Returns thread state i.e. could be thread deleted or not
  [[nodiscard]] bool CouldBeDeleted() noexcept;

  [[nodiscard]] volatile api::ThreadSignals &GetSignals() volatile noexcept;

private:
  api::DeferThread *thread_;
  volatile api::ThreadSignals signals_; // associated thread signals
  // for DeferThreadWrapper synchronization
  volatile bool will_be_deleted_;
  api::Atomic<unsigned char> nreferences_;
  api::Mutex mutex_;
};
} // namespace impl
#endif // APPLICATION_IMPLDETAILS_THREADDESCRIPTOR_HPP_
