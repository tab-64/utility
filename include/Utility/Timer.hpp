#ifndef __TIMER_HPP__
#define __TIMER_HPP__

#include <memory>
#include <thread>

namespace Util{

class Timer{
public:
    Timer(const Timer&) = delete;
    Timer(Timer&& rv){

    }

private:
    std::unique_ptr<std::thread> thread_;
    
};

}

#endif // __TIMER_HPP__