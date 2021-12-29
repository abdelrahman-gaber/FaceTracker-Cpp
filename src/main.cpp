#include <iostream>
#include "opencv2/imgproc.hpp"
#include "Detector.h"

int main(int argc, const char** argv)
{
    cv::CommandLineParser parser(argc, argv,
                             "{help h||}"
                             "{face_cascade| ../models/haarcascades/haarcascade_frontalface_alt.xml | Path to face cascade.}"
                             "{camera | 0 | Camera device number.}");

    parser.about( "\nThis program detect faces in a video stream.\n");
    parser.printMessage();

    cv::String model_path = parser.get<cv::String>("face_cascade");
    int camera_device = parser.get<int>("camera");

    Detector detector = Detector(model_path, camera_device);
    detector.Run();
}