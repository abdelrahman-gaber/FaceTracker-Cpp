#ifndef DETECTORCASCADE_H
#define DETECTORCASCADE_H

#include <iostream>
#include <vector>
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"
#include "Detector.h"

/*
This class is adapted from the code in https://docs.opencv.org/3.4/db/d28/tutorial_cascade_classifier.html
*/
class DetectorCascade : public Detector
{
public:
    DetectorCascade();
    ~DetectorCascade();
    void LoadModel(cv::String&) override;
    cv::Mat Detect(const cv::Mat&) override;
    cv::Mat PreProcess(const cv::Mat&) override;
    void Visualize(cv::Mat&, cv::Mat&, float) override;

private:
    cv::CascadeClassifier _face_cascade;

};

#endif