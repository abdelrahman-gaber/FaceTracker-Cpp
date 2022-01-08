#include <iostream>
#include "opencv2/imgproc.hpp"
#include "Processor.h"

int main(int argc, const char** argv)
{
    cv::CommandLineParser parser(argc, argv, 
                             "{help            h ||}"
                             "{model_path      m | ../models/DNN/yunet.onnx | Path to face detection model.}"
                             "{camera          c | 0 | Camera device number.}"
                             "{use_dnn         n | 1 | Flag to use DNN models, set 1/true for true value or 0/false for false}"
                             "{display         d | 1 | Flag to display the output frames with bounding boxes (1/true or 0/false)}"
                             "{verbosity       v | 2 | verbosity level (0 - 2): higher value prints more information while running}");

    parser.about( "\nThis program detect faces in a video stream.\n");
    parser.printMessage();

    cv::String model_path = parser.get<cv::String>("model_path");
    int camera_device = parser.get<int>("camera");
    bool use_dnn = parser.get<int>("use_dnn");
    bool display = parser.get<int>("display");
    int verbosity_level = parser.get<int>("verbosity");

    Processor main_processor;
    main_processor.SetParams(model_path, camera_device, verbosity_level, use_dnn, display);
    main_processor.Run();
}