#include "DetectorCascade.h"

DetectorCascade::DetectorCascade() {}
DetectorCascade::~DetectorCascade() {}

void DetectorCascade::LoadModel(cv::String& model_pth) {
    _face_cascade.load(model_pth);
}

cv::Mat DetectorCascade::PreProcess(const cv::Mat& frame) {
    // Pre-processing needed for the face Processor
    cv::Mat frame_gray;
    cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(frame_gray, frame_gray);
    return frame_gray;
}

// Detect faces and return cv::Mat of bounding boxes
cv::Mat DetectorCascade::Detect(const cv::Mat& frame) {
    //std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::vector<cv::Rect> bboxes;
    _face_cascade.detectMultiScale(frame, bboxes);

    cv::Mat faces;
    cv::Mat face(1, 4, CV_32FC1);
    for (size_t i = 0; i < bboxes.size(); i++) {
        //cv::rectangle(frame, bboxes[i], cv::Scalar(0, 255, 0), 4);
        face.at<float>(0, 0) = bboxes[i].x;
        face.at<float>(0, 1) = bboxes[i].y;
        face.at<float>(0, 2) = bboxes[i].width;
        face.at<float>(0, 3) = bboxes[i].height;
        faces.push_back(face);
    }

    return faces;
}

void DetectorCascade::Visualize(cv::Mat& img, cv::Mat& faces, double fps)
{
    //std::string fpsString = cv::format("FPS : %.2f", (float)fps);
    //cout << "FPS: " << fpsString << endl;
    int thickness = 2;
    for (int i = 0; i < faces.rows; i++)
    {
        // Print results
        std::cout << "Face " << i
             << ", top-left coordinates: (" << faces.at<float>(i, 0) << ", " << faces.at<float>(i, 1) << "), "
             << "box width: " << faces.at<float>(i, 2)  << ", box height: " << faces.at<float>(i, 3) << std::endl;
        // Draw bounding box
        auto bbox = cv::Rect2i(int(faces.at<float>(i, 0)), int(faces.at<float>(i, 1)), int(faces.at<float>(i, 2)), int(faces.at<float>(i, 3)));
        cv::rectangle(img, bbox, cv::Scalar(0, 255, 0), thickness);
    }
    //putText(img, fpsString, Point(0, 15), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);
}

