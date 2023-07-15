#ifndef DETECTOR_H_
#define DETECTOR_H_

#include "opencv2/objdetect.hpp"

// Virtual class for object detectors
class Detector {
public:
  virtual ~Detector(){};
  virtual void LoadModel(cv::String &) = 0;
  virtual cv::Mat Detect(const cv::Mat &) = 0;
  virtual cv::Mat PreProcess(const cv::Mat &) = 0;

  virtual void Visualize(cv::Mat &, cv::Mat &, float){};
};

#endif // DETECTOR_H