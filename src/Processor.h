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
#include <iomanip>
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "MessageQueue.h"
#include "Detector.h"
#include "DetectorDNN.h"
#include "DetectorCascade.h"
#include "Utils.h"

struct MessageData {
    cv::Mat img;
    cv::Mat preprocessed_img;
    cv::Mat faces;

    unsigned int frame_number;
    float capture_time;
    float preprocess_time;
    float detection_time;
    float display_time;    // the time taken for the whole system to run

    bool capture_success = false;
};


class Processor
{
public:
    Processor();
    ~Processor();
    void SetParams(cv::String, int, int, bool, bool);
    void Run();
    
private:
    void Capture();
    void Display();

    int _cam_id;
    bool _use_dnn;
    bool _display;
    int _verbosity;
    bool _is_running;
    cv::String _model_path;
    unsigned int _frame_number;
    
    cv::VideoCapture _capture;
    std::shared_ptr<Detector> _detector;
    MessageQueue< MessageData > _frame_buffer;
    MessageQueue< MessageData > _display_msg_queue;
    std::vector<std::future<void>> _futures;
    std::mutex _mtx;

    const int MAX_SIZE_FRAME_BUFFER = 3;
};

#endif