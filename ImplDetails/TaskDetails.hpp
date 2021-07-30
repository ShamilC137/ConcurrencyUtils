#ifndef APPLICATION_IMPLDETAILS_TASKDETAILS_HPP_
#define APPLICATION_IMPLDETAILS_TASKDETAILS_HPP_

// current project
#include "Utility.hpp"

namespace impl {
class BaseTask {
 public:
  BaseTask(const int *idseq = nullptr) noexcept;

  [[nodiscard]] inline const int *GetIDSequencePtr() const noexcept {
    return idseq_ptr_;
  }

 private:
  const int *idseq_ptr_;
};
}

#endif // !APPLICATION_IMPLDETAILS_TASKDETAILS_HPP_
