#ifndef APPLICATION_API_MULTITHREADING_HPP_
#define APPLICATION_API_MULTITHREADING_HPP_

// boost
#include "boost/atomic/atomic.hpp"
#include "boost/atomic/atomic_flag.hpp"

namespace api {
	template <class T>
	using Atomic = boost::atomic<T>;

	using AtomicFlag = boost::atomic_flag;
}

#endif // !APPLICATION_API_MULTITHREADING_HPP_
