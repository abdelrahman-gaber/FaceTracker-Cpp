#include <iostream>
#include "opencv2/imgproc.hpp"
#include "Processor.h"

int main(int argc, const char** argv)
{
    cv::CommandLineParser parser(argc, argv,
                             "{help h||}"
                             "{model_path m| ../models/haarcascades/haarcascade_frontalface_alt.xml | Path to face cascade.}"
                             "{camera c| 0 | Camera device number.}");

    parser.about( "\nThis program detect faces in a video stream.\n");
    parser.printMessage();

    cv::String model_path = parser.get<cv::String>("model_path");
    int camera_device = parser.get<int>("camera");

    Processor main_processor;
    main_processor.UpdateParams(model_path, camera_device);
    main_processor.Run();
}