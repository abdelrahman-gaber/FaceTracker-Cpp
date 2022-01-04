#ifndef DETECTOR_H
#define DETECTOR_H

#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>

/*
Virtual class for object detectors
*/
class Detector
{
public:
    virtual ~Detector() {};
    virtual void LoadModel(cv::String&) = 0;
    virtual cv::Mat Detect(const cv::Mat&) = 0;
    virtual cv::Mat PreProcess(const cv::Mat&) = 0;

    virtual void Visualize(cv::Mat&, cv::Mat&, float , float) {};   
     
};

#endif