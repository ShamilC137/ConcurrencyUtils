#ifndef APPLICATION_API_DATASTRUCTURES_MULTITHREADING_THREAD_HPP_
#define APPLICATION_API_DATASTRUCTURES_MULTITHREADING_THREAD_HPP_

// boost
#include "boost/thread/thread.hpp" // includes all structures to work with single  
// thread

namespace api {
using Thread = boost::thread;
using ThreadGroup = boost::thread_group;
} // namespace api
#endif
