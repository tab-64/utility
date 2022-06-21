#ifndef __THREADPOOL_HPP__
#define __THREADPOOL_HPP__

// Import standard library headers.
#include <condition_variable>
#include <deque>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace Util{

class ThreadPool{
public:
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&& rv){}
    ThreadPool(const size_t& size = 32){
        for(size_t i = 0; i < size; ++i){
            threads_.emplace_back(new std::thread([&](){
                
            }));
        }
    }

public:
    template <typename _Func, typename ..._Args>
    auto addTask(_Func func, _Args... args) -> std::future<decltype((*(_Func*)nullptr)(args...))>{
        return std::async(func, args...);
    }

private:
 
    
private:
    std::vector<std::shared_ptr<std::thread>> threads_;
}; // class ThreadPool

} // namespace Util

#endif // __THREADPOOL_HPP__