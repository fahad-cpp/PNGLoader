#ifndef TIMER
#define TIMER

#include <chrono>

class Timer{
    //TODO:make a timer for measuring performance
    std::chrono::steady_clock::time_point start;
    std::chrono::steady_clock::time_point end;
    public:
    Timer();
    ~Timer();
    void stop();
};

#endif