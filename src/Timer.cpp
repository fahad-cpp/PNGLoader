#include "Timer.h"

Timer::Timer(){
    start = std::chrono::high_resolution_clock::now();
}

void Timer::stop(){
    end = std::chrono::high_resolution_clock::now();

    //TODO:Finish the rest
}