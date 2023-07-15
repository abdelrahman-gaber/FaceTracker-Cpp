#ifndef DETECTOR_DNN_H_
#define DETECTOR_DNN_H_

#include "Detector.h"

#include "opencv2/objdetect.hpp"

// This class is adapted from the code in:
// https://docs.opencv.org/4.5.5/d0/dd4/tutorial_dnn_face.html
// You need to use OpenCV >= 4.5.3 and build it with DNN support
class DetectorDNN : public Detector {
public:
  DetectorDNN();
  ~DetectorDNN();
  void LoadModel(cv::String &) override;
  cv::Mat Detect(const cv::Mat &) override;
  cv::Mat PreProcess(const cv::Mat &) override;
  void Visualize(cv::Mat &, cv::Mat &, float) override;

private:
  void PostProcess(cv::Mat &);

  float score_threshold = 0.85;
  float nms_threshold = 0.5;
  int top_k = 500;
  cv::Size _model_input_size = cv::Size(320, 240);
  std::pair<float, float> _resize_factor;

  cv::Ptr<cv::FaceDetectorYN> fd_model;
};

#endif // DETECTOR_DNN_H_