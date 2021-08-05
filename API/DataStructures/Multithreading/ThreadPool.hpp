#ifndef APPLICATION_API_DATASTRUCTURES_MULTITHREADING_THREADPOOL_HPP_
#define APPLICATION_API_DATASTRUCTURES_MULTITHREADING_THREADPOOL_HPP_

// current project
#include "../../../Config.hpp"

#if WINDOWS32 | WINDOWS64
// microsoft
#include <SDKDDKVer.h> // Windows dependent macroses
#endif

// boost
#include "boost/asio/thread_pool.hpp"
namespace api {
using ThreadPool = boost::asio::thread_pool;
}

#endif //! APPLICATION_API_DATASTRUCTURES_MULTITHREADING_THREADPOOL_HPP_
