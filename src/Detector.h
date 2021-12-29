#ifndef DETECTOR_H
#define DETECTOR_H

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
#include "MessageQueue.h"

/*
This class is adapted from the code in https://docs.opencv.org/3.4/db/d28/tutorial_cascade_classifier.html
*/
class Detector
{
public:
    Detector(cv::String&, int);
    ~Detector();
    void Run();
    

private:
    void LoadModel(cv::String&);
    void Capture();
    std::vector<cv::Rect> Detect(const cv::Mat&);
    void Display();

    int _cam_id;
    bool _is_running;
    cv::VideoCapture _capture;
    cv::CascadeClassifier _face_cascade;
    MessageQueue< std::pair<cv::Mat, std::vector<cv::Rect>> > _display_msg_queue;
    MessageQueue< std::pair<cv::Mat, cv::Mat> > _frame_buffer;
    std::vector<std::future<void>> _futures;
    std::mutex _mtx;

    const int MAX_SIZE_FRAME_BUFFER = 2;
};

#endif