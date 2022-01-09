# FaceTracker

## Introduction

## Installation

### Dependencies

* Git and git-lfs `sudo apt install git git-lfs`
* Any C++ compiler supporting C++17
* OpenCV >= 4.5.3
  * Make sure to build OpenCV with DNN support

### Build instructions

1. Clone this repo.
2. Create a build directory in the top level directory: `mkdir build && cd build`
3. Compile: `cmake .. && make` .. you can use `-j` to use all CPU core, for example `make -j12`  
4. Connect your webcam and run the application specifying your camera id: `./face_tracker -c={YOUR_CAMERA_ID}`, for example `./face_tracker --c=0`.
5. To stop the application, press Esc
