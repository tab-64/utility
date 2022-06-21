#ifndef __THREADPOOL_HPP__
#define __THREADPOOL_HPP__

// Import standard library headers.
#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <thread>


namespace FlexChat{

namespace Util{

template <typename T>
using Result = std::future<T>;

class ThreadPool{
public:
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&& rv){}

    
private:

};

} // namespace Util
} // namespace FlexChat

#endif