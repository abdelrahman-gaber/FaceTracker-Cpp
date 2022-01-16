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
    void SetMaxQueueSize(int);

private:
    void UpdateQueue(float);
    float GetAverage();

    int _max_size=30;
    std::deque<float> _queue;
};

class AverageLatencies 
{
public:
    float avg_capture_time;
    float avg_preprocess_time;
    float avg_detection_time;
    float avg_overall_time;

    void Update(float cap, float preprocess, float detect, float overall){
        avg_capture_time = ma_capture.GetNext(cap);
        avg_preprocess_time = ma_preprocess.GetNext(preprocess);
        avg_detection_time = ma_detection.GetNext(detect);
        avg_overall_time = ma_overall.GetNext(overall);
    }

private:
    MovingAverage ma_capture;
    MovingAverage ma_preprocess;
    MovingAverage ma_detection;
    MovingAverage ma_overall;
};

