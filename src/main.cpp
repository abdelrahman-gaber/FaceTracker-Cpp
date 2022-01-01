#include <iostream>
#include "opencv2/imgproc.hpp"
#include "Processor.h"

int main(int argc, const char** argv)
{
    cv::CommandLineParser parser(argc, argv, 
                             "{help            h||}"
                             "{model_path      m| ../models/DNN/yunet.onnx | Path to face detection model.}"
                             "{camera          c| 0 | Camera device number.}"
                             "{use_dnn         d| 1 | Flag to use DNN models, set 1 for true or 0 for false}");

    parser.about( "\nThis program detect faces in a video stream.\n");
    parser.printMessage();

    cv::String model_path = parser.get<cv::String>("model_path");
    int camera_device = parser.get<int>("camera");
    int use_dnn = parser.get<int>("use_dnn");

    if (use_dnn > 1 or use_dnn < 0) {
        std::cout<< "argument --use_dnn must be 0 or 1, defaulting to 0 \n";
        use_dnn = 0;
    }

    bool dnn = false;
    if (use_dnn == 1) {dnn=true;}
    else {dnn=false;}

    std::cout << dnn << "\n";

    Processor main_processor;
    main_processor.SetParams(model_path, camera_device, dnn);
    main_processor.Run();
}