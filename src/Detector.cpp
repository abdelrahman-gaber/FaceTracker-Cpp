#include "Detector.h"
#include "MessageQueue.h"

Detector::Detector(cv::String& model_pth, int camera_id) : _cam_id(camera_id) {
    LoadModel(model_pth);
}

Detector::~Detector() {
    std::for_each(_futures.begin(), _futures.end(), [](std::future<void> &ftr) {
        ftr.wait();
    });
    _capture.release();
    cv::destroyAllWindows();
}

void Detector::LoadModel(cv::String& model_pth) {
    _face_cascade.load(model_pth);
}

void Detector::Run() {

    std::unique_lock<std::mutex> uLock(_mtx);
    _is_running = true;
    std::cout << "Detection thread #" << std::this_thread::get_id() << "\n";
    uLock.unlock();

    // Start the straem capture thread 
    _futures.emplace_back(std::async(std::launch::async, &Detector::Capture, this));
    // Start the display thread
    _futures.emplace_back(std::async(std::launch::async, &Detector::Display, this));

    while(true) {
        uLock.lock();
        if( !_is_running ) {
            uLock.unlock();
            break;}
        uLock.unlock();

        // Get the next frame from _frame_buffer queue
        std::pair<cv::Mat, cv::Mat> imgs_pair = _frame_buffer.Receive();
        cv::Mat frame_original = std::move(imgs_pair.first);
        cv::Mat frame_gray = std::move(imgs_pair.second);

        // Detect faces
        std::vector<cv::Rect> faces = Detect(frame_gray);

        // use original colored frame for display
        std::pair<cv::Mat, std::vector<cv::Rect>> img_rect_pair = std::make_pair(std::move(frame_original), std::move(faces));  
        _display_msg_queue.Send( std::move(img_rect_pair) );
        //std::cout << "display_msg_queue size:" << _display_msg_queue.GetQueueSize() << "\n";
    }
}

// This function capture new frames from the camera and apply pre-processing needed for the detector
void Detector::Capture() {
    std::unique_lock<std::mutex> uLock(_mtx);
    std::cout << "Stream capture worker thread #" << std::this_thread::get_id() << "\n";
    uLock.unlock();

    //Read the video stream
    _capture.open(_cam_id);
    if (! _capture.isOpened()) {
        std::cout << "Error opening video capture \n";
        return;
    }

    // set max frame size to 2 so you get the latest frames from the camera whatever the speed of the detector
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

        // Pre-processing needed for the face detector
        cv::Mat frame_gray;
        cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
        equalizeHist(frame_gray, frame_gray);

        // push the frames to _frame_buffer
        std::pair<cv::Mat, cv::Mat> imgs_pair = std::make_pair(std::move(frame), std::move(frame_gray));  
        _frame_buffer.Send(std::move(imgs_pair));
        //std::cout << "frame_buffer size:" << _frame_buffer.GetQueueSize() << "\n";
    }
}

// Detect faces and return vector of bounding boxes
std::vector<cv::Rect> Detector::Detect(const cv::Mat& frame) {
    //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::vector<cv::Rect> faces;
    _face_cascade.detectMultiScale(frame, faces);
    return faces;
}

void Detector::Display(){

    std::unique_lock<std::mutex> uLock(_mtx);
    std::cout << "Display worker thread #" << std::this_thread::get_id() << "\n";
    uLock.unlock();

    //cv::VideoWriter video("capture_record.mp4", cv::VideoWriter::fourcc('a','v','c','1'), 24, cv::Size(640, 480));

    cv::namedWindow("Concurrent Face Detector");
    while(true) {
        std::pair<cv::Mat, std::vector<cv::Rect>> message = _display_msg_queue.Receive();
        cv::Mat frame = std::move(message.first); 
        std::vector<cv::Rect> bboxes = std::move(message.second);

        // Draw the bounding boxes on the image frame
        // Note that this fucntion will modify the frame passed 
        for (size_t i = 0; i < bboxes.size(); i++) {
            cv::rectangle(frame, bboxes[i], cv::Scalar(0, 255, 0), 4);
        }

        //video.write(frame);
        // Display the detection
        cv::imshow("Concurrent Face Detector", frame);

        // Stop when Esc key is pressed
        if(cv::waitKey(1) == 27){
            uLock.lock();
            _is_running = false;
            uLock.unlock();
            //video.release();
            break;
        }
    }
}
