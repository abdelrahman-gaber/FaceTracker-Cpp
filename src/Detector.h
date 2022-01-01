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
#include <chrono>

/*
Virtual class for object detectors
*/
class Detector
{
public:
    virtual void LoadModel(cv::String&) {};
    virtual cv::Mat Detect(const cv::Mat&) {};
    virtual cv::Mat PreProcess(const cv::Mat&) {};
    virtual void PostProcess(cv::Mat&) {};
    virtual void Visualize(cv::Mat& img, cv::Mat& faces, double fps) {};   
     
};

#endif