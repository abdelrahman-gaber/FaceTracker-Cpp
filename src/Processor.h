#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <thread>
#include <future>
#include <memory>
#include <chrono>
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "MessageQueue.h"
#include "Detector.h"
#include "DetectorDNN.h"
#include "DetectorCascade.h"


class Processor
{
public:
    Processor();
    ~Processor();
    void SetParams(cv::String, int, bool);
    void Run();
    
private:
    void Capture();
    void Display();

    int _cam_id;
    bool _use_dnn;
    bool _is_running;
    Detector *_detector;
    cv::VideoCapture _capture;
    cv::String _model_path;
    MessageQueue< std::pair<cv::Mat, cv::Mat> > _display_msg_queue;
    MessageQueue< std::pair<cv::Mat, cv::Mat> > _frame_buffer;
    std::vector<std::future<void>> _futures;
    std::mutex _mtx;

    const int MAX_SIZE_FRAME_BUFFER = 2;
};

#endif