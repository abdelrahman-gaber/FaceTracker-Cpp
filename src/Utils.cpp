#include "Utils.h"

Timer::Timer() {
    _start = std::chrono::steady_clock::now();
}

Timer::~Timer() {}

void Timer::Tic() {
    _start = std::chrono::steady_clock::now();
}

float Timer::Toc() {
    _end = std::chrono::steady_clock::now();
    std::chrono::duration<float> duration = _end -_start;

    // return elapsed time in seconds
    return duration.count();
}
