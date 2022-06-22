#ifndef __THREADPOOL_HPP__
#define __THREADPOOL_HPP__

// Import standard library headers.
#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <iostream>
namespace Util{

class ThreadPool{
private:
    using _flag_bool       = std::shared_ptr<std::atomic<bool> >;
    using _packaged_thread = std::pair<std::unique_ptr<std::thread>, _flag_bool>;

public:
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;

    ThreadPool(const size_t& size = 32){
        addThread(size);
    }

    ~ThreadPool(void){
        std::unique_lock<std::mutex> lock(mtx_thread_);
        for(auto i = threads_.begin(); i != threads_.end(); ++i)
            i->second->store(true); // All the threads have to stop.
        cv_.notify_all(); // It's time to stop.
        for(auto i = threads_.begin(); i != threads_.end(); ++i)
            if(i->first->joinable())
                i->first->join();   // Make sure that all the threads were exited.
    }

    template <typename _Func, typename ..._Args>
    auto addTask(_Func&& func, _Args&&... args) 
        -> std::future<decltype((*(_Func*)nullptr)(args...))>{
        using _return_type = decltype((*(_Func*)nullptr)(args...));
        using _func_type   = _return_type(void);

        auto task = \
            std::make_shared<std::packaged_task<_func_type>>(std::bind(std::forward<_Func>(func), std::forward<_Args>(args)...));
        std::future<_return_type> fut = task->get_future();

        std::unique_lock<std::mutex> lock(mtx_queue_);
        tasks_.emplace_back([task](){(*task)();});
        lock.unlock();

        cv_.notify_one();
        return fut;
    } // addTask

    
    void addThread(const size_t& count){
        _flag_bool flag(new std::atomic<bool>(false));
        std::unique_lock<std::mutex> lock_thread(mtx_thread_);
        for(size_t i = 0; i < count; ++i){
            threads_.emplace_back(new std::thread([&](_flag_bool&& flag_stop){
                while(true){
                    std::unique_lock<std::mutex> lock(mtx_queue_);
                    cv_.wait(lock, [&]()->bool{return (!tasks_.empty() || flag_stop->load());});
                    
                    if(tasks_.empty()){
                        if(flag_stop->load())
                            return;
                    }
                    else{
                        auto task = tasks_.front(); // Copying, instead of moving.
                        tasks_.pop_front();
                        lock.unlock();
                        task();
                    }
                }
            }, flag), flag);

        } // Creating threads.

    } // addThread


    size_t numTask(void){
        std::unique_lock<std::mutex> lock(mtx_queue_);
        return tasks_.size();
    }

    size_t numThread(void){
        std::unique_lock<std::mutex> lock(mtx_thread_);
        return threads_.size();
    }

private:
    std::vector<_packaged_thread> threads_;
    std::deque<std::function<void(void)>> tasks_;
    std::condition_variable cv_;
    std::mutex mtx_queue_;
    std::mutex mtx_thread_;

}; // class ThreadPool

} // namespace Util

#endif // __THREADPOOL_HPP__