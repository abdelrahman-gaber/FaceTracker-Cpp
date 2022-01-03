#include "Processor.h"
#include "MessageQueue.h"

Processor::Processor(){}

Processor::~Processor() {
    std::for_each(_futures.begin(), _futures.end(), [](std::future<void> &ftr) {
        ftr.wait();
    });    
    cv::destroyAllWindows();    
    _capture.release();
    std::cout<<"FaceTracker closed!\n";
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
        _detector = std::make_shared<DetectorDNN>();
    }
    else {
        _detector = std::make_shared<DetectorCascade>();
    }

    _detector->LoadModel(_model_path);

    // Start the stream capture thread 
    _futures.emplace_back(std::async(std::launch::async, &Processor::Capture, this));
    // Start the display thread
    _futures.emplace_back(std::async(std::launch::async, &Processor::Display, this));

    Timer timer;
    while(true) {
        uLock.lock();
        if( !_is_running ) {
            uLock.unlock();
            break;
        }
        uLock.unlock();

        // Get the next frame from _frame_buffer queue
        MessageData data_msg = _frame_buffer.Receive();
        if(!data_msg.capture_success) {continue;}
        
        cv::Mat preprocessed_frame = std::move(data_msg.preprocessed_img);

        timer.Tic();
        cv::Mat faces = _detector->Detect(preprocessed_frame); 

        data_msg.detection_time = timer.Toc();
        data_msg.faces = std::move(faces);
        _display_msg_queue.Send( std::move(data_msg) );
        //std::cout << "display_msg_queue size:" << _display_msg_queue.GetQueueSize() << "\n";
    }
}

// This function captures new frames from the camera and apply pre-processing
void Processor::Capture() {
    std::unique_lock<std::mutex> uLock(_mtx);
    std::cout << "Stream capture worker thread #" << std::this_thread::get_id() << "\n";
    uLock.unlock();

    //Read the video stream
    _capture.open(_cam_id);
    if (! _capture.isOpened()) {
        std::cout << "Error opening video capture \n";
        uLock.lock();
        _is_running = false;
        uLock.unlock();
        return;
    }

    //_capture.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    _capture.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    _capture.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    _capture.set(cv::CAP_PROP_FPS, 30);

    // set max frame size to 2 so you get the latest frames from the camera whatever the speed of the Processor
    _frame_buffer.SetMaxQueueSize(MAX_SIZE_FRAME_BUFFER);
    Timer timer;
 
    while(true) {
        uLock.lock();
        if( !_is_running ) {
            uLock.unlock();
            break;
        }
        uLock.unlock();

        cv::Mat frame;
        _capture.read(frame);
        if(frame.empty()) {
            std::cout << "No captured frame -- Break! \n";
            uLock.lock();
            _is_running = false;
            uLock.unlock();
            break;
        }

        timer.Tic();
        // preprocessing for the input frames
        cv::Mat preprocessed_frame = _detector->PreProcess(frame);
        float preprocess_time = timer.Toc();
        //std::cout << "Preprocess time: " << preprocess_time*1000 << "ms \n";

        MessageData msg;
        msg.capture_success = true;
        msg.img = std::move(frame);
        msg.preprocessed_img = std::move(preprocessed_frame);
        msg.preprocess_time = preprocess_time;
        _frame_buffer.Send(std::move(msg));  // push the frames to _frame_buffer
        //std::cout << "frame_buffer size:" << _frame_buffer.GetQueueSize() << "\n";
    }
}

void Processor::Display() {
    std::unique_lock<std::mutex> uLock(_mtx);
    std::cout << "Display worker thread #" << std::this_thread::get_id() << "\n";
    uLock.unlock();

    Timer timer;
    //cv::VideoWriter video("capture_record.mp4", cv::VideoWriter::fourcc('a','v','c','1'), 24, cv::Size(640, 480));
    cv::namedWindow("FaceTracker");
    while(true) {
        uLock.lock();
        if( !_is_running ) {
            uLock.unlock();
            break;
        }
        uLock.unlock();

        MessageData message = _display_msg_queue.Receive();
        if(!message.capture_success) {continue;}

        cv::Mat frame = std::move(message.img); 
        cv::Mat faces = std::move(message.faces);
        double det_time = message.detection_time;

        // Note: this function will modify the original image
        _detector->Visualize(frame, faces, det_time);

        float elapsed_seconds = timer.Toc();
        cv::String dispaly_fps_string = cv::format("Display FPS: %3.2f", 1.0/elapsed_seconds);
        cv::putText(frame, dispaly_fps_string, cv::Point(20,40), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255,255,255), 2, false);
        timer.Tic();

        //video.write(frame);
        cv::imshow("FaceTracker", frame);

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
