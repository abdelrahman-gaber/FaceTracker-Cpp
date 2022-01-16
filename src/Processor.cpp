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


void Processor::SetParams(cv::String model_pth, int camera_id, int verbosity, bool use_dnn, bool display){
    _model_path = model_pth;
    _cam_id = camera_id;
    _use_dnn = use_dnn;
    _display = display;
    _verbosity = verbosity;
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
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
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
    _capture.open(_cam_id, cv::CAP_V4L2);
    if (! _capture.isOpened()) {
        std::cout << "Error opening video capture \n";
        uLock.lock();
        _is_running = false;
        uLock.unlock();
        return;
    }

    _capture.set(cv::CAP_PROP_BUFFERSIZE, 2); // to get the latest frame
    _capture.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    _capture.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    _capture.set(cv::CAP_PROP_FPS, 30);
    //_capture.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    int fourcc = _capture.get(cv::CAP_PROP_FOURCC);
    std::string fourcc_str = cv::format("%c%c%c%c", fourcc & 255, (fourcc >> 8) & 255, (fourcc >> 16) & 255, (fourcc >> 24) & 255);
    std::cout << "Input video format: " << fourcc_str << "\n";

    // set max frame size to 2 so you get the latest frames from the camera whatever the speed of the Processor
    _frame_buffer.SetMaxQueueSize(MAX_SIZE_FRAME_BUFFER);
    _frame_number = 0;
    Timer timer;
    float capture_time;
    float preprocess_time;
 
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        uLock.lock();
        if( !_is_running ) {
            uLock.unlock();
            break;
        }
        uLock.unlock();

        cv::Mat frame;
        timer.Tic();
        _capture.read(frame);
        capture_time = timer.Toc();
        if(frame.empty()) {
            std::cout << "No captured frame -- Break! \n";
            uLock.lock();
            _is_running = false;
            uLock.unlock();
            break;
        }

        _frame_number += 1;
        timer.Tic();
        // preprocessing for the input frames
        cv::Mat preprocessed_frame = _detector->PreProcess(frame);
        preprocess_time = timer.Toc();

        MessageData msg;
        msg.capture_success = true;
        msg.frame_number = _frame_number;
        msg.img = std::move(frame);
        msg.preprocessed_img = std::move(preprocessed_frame);
        msg.capture_time = capture_time;
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
    float detection_time;
    float display_time;
    AverageLatencies avg_times;

    std::cout << std::setprecision(2) << std::fixed;

    //cv::VideoWriter video("capture_record.mp4", cv::VideoWriter::fourcc('a','v','c','1'), 24, cv::Size(640, 480));
    if(_display) {cv::namedWindow("FaceTracker");}
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
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
        detection_time = message.detection_time + message.preprocess_time;
        display_time = timer.Toc();
        timer.Tic();
 
        avg_times.Update(message.capture_time, message.preprocess_time, message.detection_time, display_time);

        if(_verbosity >= 2) {
            std::cout << "Frame " << message.frame_number << " : Capture: " << message.capture_time*1000 
                  << " ms .. Preprocessing: " << message.preprocess_time*1000 << " ms .. Detection: " 
                  << message.detection_time*1000 << " ms .. Display: " << display_time*1000 
                  << " ms ... Avg display time: " << avg_times.avg_overall_time*1000 << " ms \n";
        }

        // each 100 frames
        if(_verbosity >= 1 and message.frame_number%100==0) {
            std::cout << "Frame " << message.frame_number << " : Avg capture time: " << avg_times.avg_capture_time*1000
            << " ms .. Avg preprocess time: " << avg_times.avg_preprocess_time*1000 
            << " ms .. Avg detection time: " << avg_times.avg_detection_time*1000
            << " ms .. Avg overall time: " << avg_times.avg_overall_time*1000 
            << " ms --> " << 1.0/avg_times.avg_overall_time << " fps \n";
        }

        // Note: this function will modify the original image
        _detector->Visualize(frame, faces, avg_times.avg_overall_time);

        //video.write(frame);
        if(_display) {
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
}
