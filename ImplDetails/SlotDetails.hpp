#ifndef APPLICATION_IMPLDETAILS_SLOTDETAILS_HPP_
#define APPLICATION_IMPLDETAILS_SLOTDETAILS_HPP_
// current project
#include "TaskDetails.hpp"
#include "../API/DataStructures/TaskWrapper.hpp"
#include "Utility.hpp"

namespace impl {
// Basic slot which contains RTTI about derived classes, its priority (if
// given), and operator () that calls correct underlying function with given
// task.
class BaseSlot {
public:
  // idseq - derived class types identifiers sequence;
  // retid - derived class return type identifier;
  // priority - slot priority;
  BaseSlot(const int *idseq, const int *retid,
           const int priority = -1) noexcept;

  inline virtual ~BaseSlot() noexcept {}

  BaseSlot &operator=(const BaseSlot &rhs) = delete;
  BaseSlot &operator=(BaseSlot &&rhs) = delete;

  // Return the size in bytes of this class; used to correct deallocate object
  // of this class which captured with pointer to base.
  [[nodiscard]] inline virtual std::size_t SizeInBytes() const noexcept {
    return sizeof(BaseSlot);
  }

  // Returns derived class types identifiers sequence
  [[nodiscard]] inline const int *GetIDSequencePtr() const noexcept {
    return idseq_ptr_;
  }

  // Return derived class return type identifier
  [[nodiscard]] inline const int *GetRetIDPtr() const noexcept {
    return retid_ptr_;
  }

  // Sets the slot priority
  inline void SetPriority(const int priority) noexcept { priority_ = priority; }

  // Return current priority
  [[nodiscard]] inline int GetPriority() const noexcept { return priority_; }

  // Calls underlying function
  // throws
  void operator()(api::TaskWrapper &task) noexcept(false);
  void operator()(api::TaskWrapper &&task) noexcept(false);

protected:
  // potentially throws
  virtual void RealCall(api::TaskWrapper &task) noexcept(false) = 0;

private:
  const int *idseq_ptr_; // derived class types identifiers sequence
  const int *retid_ptr_; // derived class return type identifier
  int priority_; //  priority - slot priority; slot with the highest prioprity
                 //  called first; if no priority is set, -1 is used.
};
} // namespace impl

#endif // !APPLICATION_IMPLDETAILS_SLOTDETAILS_HPP_
