#ifndef APPLICATION_IMPLDETAILS_SLOTDETAILS_HPP_
#define APPLICATION_IMPLDETAILS_SLOTDETAILS_HPP_

// current project
#include "TaskDetails.hpp"
#include "Utility.hpp"

namespace impl {
class BaseSlot {
public:
  BaseSlot(const int *idseq = nullptr) noexcept;

  [[nodiscard]] inline const int *GetIDSequencePtr() const noexcept {
    return idseq_ptr_;
  }

  void operator()(BaseTask *task) noexcept(false);

protected:
  virtual void RealCall(BaseTask *task) noexcept(false) = 0;

private:
  const int *idseq_ptr_;
};
} // namespace impl

#endif // !APPLICATION_IMPLDETAILS_SLOTDETAILS_HPP_
