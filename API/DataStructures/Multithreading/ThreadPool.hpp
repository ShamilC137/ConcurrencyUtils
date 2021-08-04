#ifndef APPLICATION_API_DATASTRUCTURES_MULTITHREADING_THREADPOOL_HPP_
#define APPLICATION_API_DATASTRUCTURES_MULTITHREADING_THREADPOOL_HPP_

// boost
//#include <SDKDDKVer.h>
#ifndef _WIN32_WINNT
// Windows version
#define _WIN32_WINNT 0x0601 // from header above; only this define is used
                            // so I manually defined it
#endif

#include "boost/asio/thread_pool.hpp"
namespace api {
using ThreadPool = boost::asio::thread_pool;
}

#endif //! APPLICATION_API_DATASTRUCTURES_MULTITHREADING_THREADPOOL_HPP_
