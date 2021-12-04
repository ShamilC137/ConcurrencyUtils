#ifndef APPLICATION_API_DATASTRUCTURES_MULTITHREADING_SHAREDLOCK_HPP_
#define APPLICATION_API_DATASTRUCTURES_MULTITHREADING_SHAREDLOCK_HPP_

#include "boost/thread/shared_lock_guard.hpp"

namespace api {
template <class SharedMutex>
using SharedLockGuard = typename boost::shared_lock_guard<SharedMutex>;
}

#endif  // APPLICATION_API_DATASTRUCTURES_MULTITHREADING_SHAREDLOCK_HPP_
