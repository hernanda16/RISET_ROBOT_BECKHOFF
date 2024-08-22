cmake_minimum_required(VERSION 3.0.2)
project(iris_msgs)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  message_generation
  std_msgs
  geometry_msgs
)

## Generate messages in the 'msg' folder
add_message_files(
  FILES
  bs2pc.msg
  stm2pc.msg
  pc2stm.msg
  vision_status.msg
  data_robots.msg
  goalpost.msg
)

## Generate services in the 'srv' folder
add_service_files(
  FILES
  CameraConfig.srv
  YuvThresholdInit.srv
  CamOffset.srv
)

## Generate added messages and services with any dependencies listed here
generate_messages(
  DEPENDENCIES
  std_msgs
  geometry_msgs
)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES iris_msgs
#  CATKIN_DEPENDS message_generation std_msgs
#  DEPENDS system_lib
)

include_directories(
# include
  ${catkin_INCLUDE_DIRS}
)