#ifndef TIMER
#define TIMER

#include <chrono>

class Timer {
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;

  public:
    double dtms;
    Timer();
    ~Timer();
    void stop();
};

#endif
