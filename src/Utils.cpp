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


MovingAverage::MovingAverage(){};

MovingAverage::~MovingAverage(){};

float MovingAverage::GetNext(float new_value){
    UpdateQueue(new_value);
    return GetAverage();
}

void MovingAverage::UpdateQueue(float value){
    if (_queue.size() < _max_size) {
        _queue.emplace_back(value);
    }
    else {
        // remove one element from the front and add the new elemnt to the back of the queue
        _queue.pop_front();
        _queue.emplace_back(value);
    }
}

float MovingAverage::GetAverage() {
    float sum = 0.0;
    for (const float& elmnt : _queue){
        sum += elmnt;
    }
    return sum/_queue.size();
}
