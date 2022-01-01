#include "Processor.h"
#include "MessageQueue.h"

Processor::Processor(){}

Processor::~Processor() {
    std::for_each(_futures.begin(), _futures.end(), [](std::future<void> &ftr) {
        ftr.wait();
    });
    _capture.release();
    cv::destroyAllWindows();
    delete _detector;
}

void Processor::SetParams(cv::String model_pth, int camera_id, bool use_dnn){
    _model_path = model_pth;
    _cam_id = camera_id;
    _use_dnn = use_dnn;
}

void Processor::Run() {

    std::unique_lock<std::mutex> uLock(_mtx);
    _is_running = true;
    std::cout << "Detection thread #" << std::this_thread::get_id() << "\n";
    uLock.unlock();

    if(_use_dnn) {
        _detector = new DetectorDNN;
    }
    else {
        _detector = new DetectorCascade;
    }

    _detector->LoadModel(_model_path);

    // Start the straem capture thread 
    _futures.emplace_back(std::async(std::launch::async, &Processor::Capture, this));
    // Start the display thread
    _futures.emplace_back(std::async(std::launch::async, &Processor::Display, this));

    while(true) {
        uLock.lock();
        if( !_is_running ) {
            uLock.unlock();
            break;
        }
        uLock.unlock();

        // Get the next frame from _frame_buffer queue
        std::pair<cv::Mat, cv::Mat> imgs_pair = _frame_buffer.Receive();
        cv::Mat frame_original = std::move(imgs_pair.first);
        cv::Mat preprocessed_frame = std::move(imgs_pair.second);

        auto detection_time_start = std::chrono::steady_clock::now();
        cv::Mat faces = _detector->Detect(preprocessed_frame); 
        auto detection_time_end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = detection_time_end - detection_time_start;
        std::cout << "Detection FPS: " << 1.0/elapsed_seconds.count() << "\n";

        // use the original frame for display
        std::pair<cv::Mat, cv::Mat> img_rect_pair = std::make_pair(std::move(frame_original), std::move(faces));  
        _display_msg_queue.Send( std::move(img_rect_pair) );
        //std::cout << "display_msg_queue size:" << _display_msg_queue.GetQueueSize() << "\n";
    }
}

// This function capture new frames from the camera and apply pre-processing
void Processor::Capture() {
    std::unique_lock<std::mutex> uLock(_mtx);
    std::cout << "Stream capture worker thread #" << std::this_thread::get_id() << "\n";
    uLock.unlock();

    //Read the video stream
    _capture.open(_cam_id);
    if (! _capture.isOpened()) {
        std::cout << "Error opening video capture \n";
        return;
    }

    //_capture.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    _capture.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    _capture.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    _capture.set(cv::CAP_PROP_FPS, 30);

    // set max frame size to 2 so you get the latest frames from the camera whatever the speed of the Processor
    _frame_buffer.SetMaxQueueSize(MAX_SIZE_FRAME_BUFFER);
 
    while(true) {
        uLock.lock();
        if( !_is_running ) {break;}
        uLock.unlock();

        cv::Mat frame;
        _capture.read(frame);
        if(frame.empty()) {
            std::cout << "No captured frame -- Break! \n";
            break;
        }

        // preprocessing for the input frames
        cv::Mat preprocessed_frame = _detector->PreProcess(frame);

        // push the frames to _frame_buffer
        std::pair<cv::Mat, cv::Mat> imgs_pair = std::make_pair(std::move(frame), std::move(preprocessed_frame));  
        _frame_buffer.Send(std::move(imgs_pair));
        //std::cout << "frame_buffer size:" << _frame_buffer.GetQueueSize() << "\n";
    }
}

void Processor::Display() {
    std::unique_lock<std::mutex> uLock(_mtx);
    std::cout << "Display worker thread #" << std::this_thread::get_id() << "\n";
    uLock.unlock();
    auto display_time_start = std::chrono::steady_clock::now();

    //cv::VideoWriter video("capture_record.mp4", cv::VideoWriter::fourcc('a','v','c','1'), 24, cv::Size(640, 480));
    cv::namedWindow("Face Tracker");
    while(true) {
        std::pair<cv::Mat, cv::Mat> message = _display_msg_queue.Receive();
        cv::Mat frame = std::move(message.first); 
        cv::Mat faces = std::move(message.second);

        _detector->Visualize(frame, faces, 0);

        auto display_time_end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = display_time_end - display_time_start;
        cv::String dispaly_fps_string = cv::format("Display FPS: %3.2f", 1.0/elapsed_seconds.count());
        cv::putText(frame, dispaly_fps_string, cv::Point(20,40), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255,255,255), 2, false);
        display_time_start = std::chrono::steady_clock::now();

        //video.write(frame);
        cv::imshow("Face Tracker", frame);

        // Stop when Esc key is pressed
        if(cv::waitKey(1) == 27){
            uLock.lock();
            _is_running = false;
            uLock.unlock();
            //video.release()
            break;
        }
    }
}
