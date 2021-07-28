#ifndef APPLICATION_API_DATASTRUCTURES_MULTIHREADING_MUTEX_HPP_
#define APPLICATION_API_DATASTRUCTURES_MULTIHREADING_MUTEX_HPP_

// boost
#include "boost/thread/mutex.hpp" // specifies cpp

namespace api {
using Mutex = boost::mutex;
} // namespace api
#endif // !APPLICATION_API_DATASTRUCTURES_MULTIHREADING_MUTEX_HPP_
