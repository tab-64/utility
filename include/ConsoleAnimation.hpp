#ifndef __CONSOLEANIMATION_HPP__
#define __CONSOLEANIMATION_HPP__

//
#include <chrono>
#include <iostream>
#include <thread>

namespace Util{

namespace Console{
    
class Animation{
public:
    static void RotatingLine(int nTurns, int nTurnsPerSecond = 2, bool isClockwise = true){
        auto&& SleepTime = std::chrono::milliseconds(250 / nTurnsPerSecond);
        std::cout << "\033[?25l";
        if(isClockwise){
            for(int i = 0; i < nTurns; ++i){
                std::cout << "|\033[1D";
                std::this_thread::sleep_for(SleepTime);
                std::cout << "/\033[1D";
                std::this_thread::sleep_for(SleepTime);
                std::cout << "-\033[1D";
                std::this_thread::sleep_for(SleepTime);
                std::cout << "\\\033[1D";
                std::this_thread::sleep_for(SleepTime);
            }
        }
        else{
            for(int i = 0; i < nTurns; ++i){
                std::cout << "|\033[1D";
                std::this_thread::sleep_for(SleepTime);
                std::cout << "\\\033[1D";
                std::this_thread::sleep_for(SleepTime);
                std::cout << "-\033[1D";
                std::this_thread::sleep_for(SleepTime);
                std::cout << "/\033[1D";
                std::this_thread::sleep_for(SleepTime);
            }
        }
        std::cout << " \033[1D";
    }
};

class ProgressBar{
public:
    ProgressBar(int x, int y){
        pos_x_ = x;
        pos_y_ = y;
    }

protected:
    int pos_x_;
    int pos_y_;
};

} // namespace Console

} // namespace Util

#endif