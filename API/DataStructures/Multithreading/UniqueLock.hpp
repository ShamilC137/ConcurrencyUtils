#ifndef APPLICATION_API_MULTITHREADING_UNIQUELOCK_HPP_
#define APPLICATION_API_MULTITHREADING_UNIQUELOCK_HPP_

// boost
#include <boost/thread/lock_types.hpp> 

namespace api {
template <class T>
using UniqueLock = boost::unique_lock<T>;
}
#endif // !APPLICATION_API_MULTITHREADING_UNIQUELOCK_HPP_
