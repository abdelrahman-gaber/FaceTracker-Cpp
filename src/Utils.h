#include <iostream>
#include <chrono>

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
