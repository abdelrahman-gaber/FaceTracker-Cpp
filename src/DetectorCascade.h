#ifndef DETECTORCASCADE_H
#define DETECTORCASCADE_H

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
This class is adapted from the code in https://docs.opencv.org/3.4/db/d28/tutorial_cascade_classifier.html
*/
class DetectorCascade
{
public:
    DetectorCascade();
    ~DetectorCascade();
    void LoadModel(cv::String&);
    std::vector<cv::Rect> Detect(const cv::Mat&);

private:
    cv::CascadeClassifier _face_cascade;

};

#endif