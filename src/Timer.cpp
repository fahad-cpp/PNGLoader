#include "Timer.h"

Timer::Timer(){
    start = std::chrono::high_resolution_clock::now();
}

void Timer::stop(){
    end = std::chrono::high_resolution_clock::now();

    long long startT = std::chrono::time_point_cast<std::chrono::milliseconds>(start).time_since_epoch().count();
    long long endT = std::chrono::time_point_cast<std::chrono::milliseconds>(end).time_since_epoch().count();

    long long dur = endT - startT;

    dtms = (double)dur;
}

Timer::~Timer(){
    
}