/**
 * @file ThreadPool.hpp
 * @author Tab (2969117392@qq.com)
 * @brief The declaration and definition of class ThreadPool
 * @version 0.1
 * @date 2022-06-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

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

namespace Util{

/**
 * @brief class ThreadPool
 * 
 */
class ThreadPool{
private:
    using _flag_bool       = std::shared_ptr<std::atomic<bool> >;
    using _packaged_thread = std::pair<std::unique_ptr<std::thread>, _flag_bool>;

public:
    /// DO NOT use copy constructor.
    ThreadPool(const ThreadPool&) = delete;
    /// DO NOT use move constructor.
    ThreadPool(ThreadPool&&) = delete;

    ThreadPool(const size_t& size = 32, const size_t& max_tasks = 0xFFFFFFFFFFFFFFFF){
        addThread(size);
        max_tasks_ = max_tasks;
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


    /**
    * @brief Add a task to the ThreadPool.
    * 
    * @param func The task to add.
    * @param args The additional parameters to pass.
    * 
    * @return The future of the task's return value.
    */
    template <typename _Func, typename ..._Args>
    auto addTask(_Func&& func, _Args&&... args) 
        -> std::future<decltype((*(_Func*)nullptr)(args...))>{

        using _return_type = decltype((*(_Func*)nullptr)(args...));
        using _func_type   = _return_type(void);
        
        auto task = \
            std::make_shared<std::packaged_task<_func_type>>(std::bind(std::forward<_Func>(func), std::forward<_Args>(args)...));
        std::future<_return_type> fut = task->get_future();

        std::unique_lock<std::mutex> lock(mtx_queue_);
        if(tasks_.size() >= max_tasks_){
            lock.unlock();
            (*task)();
            return fut;
        }
        
        tasks_.emplace_back([task](){(*task)();});
        // Invoke notify_one() when the tasks is less than the threads(or equal to).
        if(tasks_.size() <= numThread())
            cv_.notify_one();
        lock.unlock();
        
        return fut;

    } // addTask

    
    /**
     * @brief Add new threads to the ThreadPool.
     * 
     * @param count How many threads you'd like to add.
     * @param temp  Whether the new thread is temporary or not. (Default: true)
     * 
     */
    void addThread(const size_t& count){
        _flag_bool flag(new std::atomic<bool>(false)); // Use pointers to avoid deconstructing too early.
        std::unique_lock<std::mutex> lock_thread(mtx_thread_);
        for(size_t i = 0; i < count; ++i){
            threads_.emplace_back(new std::thread([&](_flag_bool flag_stop) -> void{
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


    /**
     * @brief Remove some workers.
     * 
     * @param count How many workers to remove.
     * 
     * @warning If the count you offered is larger than the count of the existing threads, 
     *       exception will be throwed.
     */
    void removeThread(const size_t& count){
        std::unique_lock<std::mutex> lock(mtx_thread_);
        if(count >= threads_.size()){ // Check
            throw "ThreadPool::removeThread(): ERROR: invalid number offered.";
            return;
        }
        for(size_t i = 0; i < count; ++i){
            threads_.back().first->detach();
            threads_.back().second->store(true);
            threads_.pop_back();
        }
    }

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
    size_t max_tasks_;
}; // class ThreadPool

} // namespace Util

#endif // __THREADPOOL_HPP__