cmake_minimum_required(VERSION 3.0.2)
project(vision)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++14)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  cv_bridge
  image_transport
  roscpp
  std_msgs
  iris_msgs
  roslib
)

find_package(OpenCV REQUIRED)
find_package(yaml-cpp REQUIRED)

## INCLUDE_DIRS: uncomment this if your package contains header files
catkin_package(
 INCLUDE_DIRS include
)

## Specify additional locations of header files
include_directories(
include
  ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
)

## Declare a C++ executable
add_executable(capture src/capture.cpp)
add_executable(detection src/detection.cpp)
add_executable(detection2 src/detection2.cpp)
add_executable(main src/main.cpp)
add_executable(web_adapter src/web_adapter.cpp)

## Specify libraries to link a library or executable target against
target_link_libraries(capture ${catkin_LIBRARIES} ${OpenCV_LIBRARIES} ${YAML_CPP_LIBRARIES})
target_link_libraries(detection ${catkin_LIBRARIES} ${OpenCV_LIBRARIES} ${YAML_CPP_LIBRARIES})
target_link_libraries(detection2 ${catkin_LIBRARIES} ${OpenCV_LIBRARIES} ${YAML_CPP_LIBRARIES})
target_link_libraries(main ${catkin_LIBRARIES})
target_link_libraries(web_adapter ${catkin_LIBRARIES} ${YAML_CPP_LIBRARIES})

add_dependencies(capture iris_msgs_generate_messages_cpp)
add_dependencies(detection iris_msgs_generate_messages_cpp)
add_dependencies(detection2 iris_msgs_generate_messages_cpp)
add_dependencies(web_adapter iris_msgs_generate_messages_cpp)