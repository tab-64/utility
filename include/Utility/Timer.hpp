/**
 * @file Timer.hpp
 * @author Tab (2969117392@qq.com)
 * @brief 
 * @version 0.1
 * @date 2022-06-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __TIMER_HPP__
#define __TIMER_HPP__

#include <chrono>
#include <functional>
#include <memory>
#include <thread>

#include "ThreadPool.hpp"


namespace Util{

template <typename _Func, typename _Duration>
class TimerTask{
public:
    TimerTask(_Func&& func, _Duration&& duration){
        task_ = func;
        duration_ = duration;
    }

    void operator()(void){
        task_();
    }

private:
    _Func task_;
    _Duration duration_;
};

/**
 * @brief 
 * 
 */
class Timer{
public:
    Timer(void){

    }
    Timer(const Timer&) = delete;
    Timer(Timer&& rv){
        thread_timer_.swap(rv.thread_timer_);
    }

    template <typename _Func, typename ..._Args>
    void addTask(const TimerTask<_Func, _Args...>& task){

    }
private:
    std::unique_ptr<std::thread> thread_timer_;
    
};

}

#endif // __TIMER_HPP__