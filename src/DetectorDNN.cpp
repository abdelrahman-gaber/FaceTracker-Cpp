#include "DetectorDNN.h"

DetectorDNN::DetectorDNN() {}
DetectorDNN::~DetectorDNN() {}

void DetectorDNN::LoadModel(cv::String& model_pth) {
    // backend_id -> 0: default, 1: Halide, 2: Intel's Inference Engine, 3: OpenCV, 4: VKCOM, 5: CUDA
    // target_id -> 0: CPU, 1: OpenCL, 2: OpenCL FP16, 3: Myriad, 4: Vulkan, 5: FPGA, 6: CUDA, 7: CUDA FP16, 8: HDDL
    // TODO: pass these values from a config file
    int backend_id = 0;  //5
    int target_id = 0;   //6
    fd_model = cv::FaceDetectorYN::create(model_pth, "", _model_input_size, score_threshold, nms_threshold, top_k, backend_id, target_id);
    //fd_model->setInputSize(_model_input_size);
}

cv::Mat DetectorDNN::PreProcess(const cv::Mat& frame) {
    cv::Mat frame_out;
    // TODO: make sure resize will keep aspect ratio, in the 4:3 input frame, this is done correctly. 
    cv::resize(frame, frame_out, _model_input_size, 0, 0, cv::INTER_LINEAR);
    _resize_factor.first  = float(_model_input_size.width) / float(frame.cols);
    _resize_factor.second = float(_model_input_size.height) / float(frame.rows);
    return frame_out;
}

void DetectorDNN::PostProcess(cv::Mat& faces) {
    float rs;
    for(int i = 0; i < faces.rows; i++) {
        for(int j = 0; j < 14; j++) {
            if (j%2==0){rs=_resize_factor.first;} // even -> x
            else {rs=_resize_factor.second;}      // odd  -> y
            faces.at<float>(i, j) = (1.0/rs)*faces.at<float>(i, j);
        }
    }
}

// Detect faces and return cv::Mat of bounding boxes and landmarks
cv::Mat DetectorDNN::Detect(const cv::Mat& frame) {
    cv::Mat faces;
    fd_model->detect(frame, faces);
    PostProcess(faces);
    return faces;
}

void DetectorDNN::Visualize(cv::Mat& img, cv::Mat& faces, double fps)
{
    //std::string fpsString = cv::format("FPS : %.2f", (float)fps);
    //cout << "FPS: " << fpsString << endl;
    int thickness = 2;
    for (int i = 0; i < faces.rows; i++)
    {
        // Print results
        std::cout << "Face " << i
             << ", top-left coordinates: (" << faces.at<float>(i, 0) << ", " << faces.at<float>(i, 1) << "), "
             << "box width: " << faces.at<float>(i, 2)  << ", box height: " << faces.at<float>(i, 3) << ", "
             << "score: " << cv::format("%.2f", faces.at<float>(i, 14))
             << std::endl;
        // Draw bounding box
        auto bbox = cv::Rect2i(int(faces.at<float>(i, 0)), int(faces.at<float>(i, 1)), int(faces.at<float>(i, 2)), int(faces.at<float>(i, 3)));
        cv::rectangle(img, bbox, cv::Scalar(0, 255, 0), thickness);
        // Draw landmarks
        cv::circle(img, cv::Point2i(int(faces.at<float>(i, 4)), int(faces.at<float>(i, 5))), 2, cv::Scalar(255, 0, 0), thickness);
        cv::circle(img, cv::Point2i(int(faces.at<float>(i, 6)), int(faces.at<float>(i, 7))), 2, cv::Scalar(0, 0, 255), thickness);
        cv::circle(img, cv::Point2i(int(faces.at<float>(i, 8)), int(faces.at<float>(i, 9))), 2, cv::Scalar(0, 255, 0), thickness);
        cv::circle(img, cv::Point2i(int(faces.at<float>(i, 10)), int(faces.at<float>(i, 11))), 2, cv::Scalar(255, 0, 255), thickness);
        cv::circle(img, cv::Point2i(int(faces.at<float>(i, 12)), int(faces.at<float>(i, 13))), 2, cv::Scalar(0, 255, 255), thickness);
    }
    //putText(img, fpsString, Point(0, 15), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);
}
