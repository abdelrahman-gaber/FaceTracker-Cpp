#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>
#include <thread>
#include <future>
#include <chrono>
#include "MessageQueue.h"
#include "Detector.h"
#include "DetectorDNN.h"

class Processor
{
public:
    Processor();
    ~Processor();
    void UpdateParams(cv::String model_pth, int camera_id);
    void Run();
    

private:
    void Capture();
    void Display();

    int _cam_id;
    bool _is_running;
    //Detector _detector;
    DetectorDNN _detector;
    cv::VideoCapture _capture;
    cv::String _model_path;
    //MessageQueue< std::pair<cv::Mat, std::vector<cv::Rect>> > _display_msg_queue;
    MessageQueue< std::pair<cv::Mat, cv::Mat> > _display_msg_queue;
    MessageQueue< std::pair<cv::Mat, cv::Mat> > _frame_buffer;
    std::vector<std::future<void>> _futures;
    std::mutex _mtx;

    const int MAX_SIZE_FRAME_BUFFER = 2;
};

#endif