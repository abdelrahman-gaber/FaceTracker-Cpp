#include <iostream>
#include <chrono>
#include <deque>

class Timer
{
public:
    Timer();
    ~Timer();
    void Tic();  // start the timer
    float Toc(); // stop the timer and return elapsed time in seconds

private:
    std::chrono::time_point<std::chrono::steady_clock> _start, _end;    
};


class MovingAverage
{
public:
    MovingAverage();
    ~MovingAverage();
    float GetNext(float);

private:
    void UpdateQueue(float);
    float GetAverage();

    int _max_size=30;
    std::deque<float> _queue;

};