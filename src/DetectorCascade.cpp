#include "DetectorCascade.h"

DetectorCascade::DetectorCascade() {}
DetectorCascade::~DetectorCascade() {}

void DetectorCascade::LoadModel(cv::String& model_pth) {
    _face_cascade.load(model_pth);
}

// Detect faces and return vector of bounding boxes
std::vector<cv::Rect> DetectorCascade::Detect(const cv::Mat& frame) {
    //std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::vector<cv::Rect> faces;
    _face_cascade.detectMultiScale(frame, faces);
    return faces;
}

