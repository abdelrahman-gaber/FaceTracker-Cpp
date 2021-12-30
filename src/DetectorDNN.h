#ifndef DETECTORDNN_H
#define DETECTORDNN_H

#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/dnn.hpp"
#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>
#include <thread>
#include <future>
#include <chrono>

/*
This class is adapted from the code in https://docs.opencv.org/4.5.5/d0/dd4/tutorial_dnn_face.html
You need to use OpenCV >= 4.5.3 and build it with DNN support
*/
class DetectorDNN
{
public:
    DetectorDNN();
    ~DetectorDNN();
    void LoadModel(cv::String&);
    cv::Mat Detect(const cv::Mat&);
    cv::Mat PreProcess(const cv::Mat&);
    void PostProcess(cv::Mat&);
    void Visualize(cv::Mat& img, cv::Mat& faces, double fps);

private:
    float score_threshold = 0.8;
    float nms_threshold = 0.3;
    int top_k = 5000;
    cv::Size _model_input_size = cv::Size(320, 240);
    std::pair<float, float> _resize_factor;

    cv::Ptr<cv::FaceDetectorYN> fd_model;
};

#endif