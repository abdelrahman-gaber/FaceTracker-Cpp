cmake_minimum_required(VERSION 3.10.2) #3.11.3

set(CMAKE_CXX_STANDARD 17)
project(face_tracker)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})
add_definitions(${OpenCV_DEFINITIONS})

# Find all executables
file(GLOB project_SRCS src/*.cpp) #src/*.h

# Add project executable
add_executable(face_tracker ${project_SRCS})
target_link_libraries(face_tracker ${OpenCV_LIBRARIES})
