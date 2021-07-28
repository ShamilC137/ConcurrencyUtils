#ifndef APPLICATION_API_MULTITHREADING_SCOPEDLOCK_HPP_
#define APPLICATION_API_MULTITHREADING_SCOPEDLOCK_HPP_

// boost
#include "boost/interprocess/sync/scoped_lock.hpp"

namespace api {
template <class T> using ScopedLock = boost::interprocess::scoped_lock<T>;
} // namespace api
#endif // !APPLICATION_API_MULTITHREADING_SCOPEDLOCK_HPP_
