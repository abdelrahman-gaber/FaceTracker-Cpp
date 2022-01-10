# FaceTracker

## Introduction

This project performs real-time face detection and tracking on video streams. The goal is to optimize the camera capture and the detection to run in parallel threads, allowing for very high frame rate.

The only dependency is OpenCV, but it requires to build OpenCV with DNN support. The object detector used is [YuNet](https://github.com/ShiqiYu/libfacedetection), which is a very fast and efficient detector. YuNet is included in OpenCV>=4.5.3 with support to run on CPU and GPU, therefore, no additional dependancies are required. This app can run on x86 processors, Raspberry pi, and NVIDIA Jetson devices. 

This project started as my submission for the final project of Udacity C++ Nanodegree [[link](https://github.com/abdelrahman-gaber/concurrent-face-detector-cpp)]. Now I am trying to optimize it and add more features, such as using DNN face detection model, and adding face tracking and re-identification.   

## Installation
### Dependencies

* Git and git-lfs `sudo apt install git git-lfs`
* Any C++ compiler supporting C++17
* OpenCV >= 4.5.3 (recommended: OpenCV 4.5.5)
  * Make sure to build OpenCV with DNN support

### Build instructions

1. Clone this repo.
2. Create a build directory in the top level directory: `mkdir build && cd build`
3. Compile: `cmake .. && make` .. you can use `-j` to use all CPU core, for example `make -j12`  
4. Connect your webcam and run the application specifying your camera id: `./face_tracker -c={YOUR_CAMERA_ID}`, for example `./face_tracker -c=0`.
5. To stop the application, press Esc

## TODO list
- [x] Implement the basic concurrent face detector with OpenCV
- [x] Add DNN face detector
- [x] Option to choose DNN or normal detector
- [x] Options to enable/disable visualization and logging
- [x] Show averag processing fps
- [x] Test on Rapsberry Pi (tested on rpi4 with 4GB of RAM)
- [ ] Test on Jetson nano
- [ ] Process video files
- [ ] Save output visualization as a video file
- [ ] Read all parameters from a config file
- [ ] Add a benchmarking option
- [ ] Implement a basic single face tracker
- [ ] Implement more advanced multi-face tracker
- [ ] Add face recognition model to allow for face re-identification

