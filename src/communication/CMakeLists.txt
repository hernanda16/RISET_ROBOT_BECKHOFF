cmake_minimum_required(VERSION 3.0.2)
project(communication)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  iris_msgs
  roslib
)

find_package(yaml-cpp REQUIRED)

## Generate messages in the 'msg' folder
# add_message_files(
#   FILES
# )

# generate_messages(
#   DEPENDENCIES
#   std_msgs
# )

## INCLUDE_DIRS: uncomment this if your package contains header files
catkin_package(
 INCLUDE_DIRS include
)

## Specify additional locations of header files
include_directories(
include
  ${catkin_INCLUDE_DIRS}
)

## Declare a C++ executable
add_executable(comm_icast src/comm_icast.cpp)
add_executable(comm_multicast src/comm_multicast.cpp)

## Specify libraries to link a library or executable target against
target_link_libraries(comm_icast ${catkin_LIBRARIES} icast)
target_link_libraries(comm_multicast ${catkin_LIBRARIES} ${YAML_CPP_LIBRARIES})

add_dependencies(comm_icast iris_msgs_generate_messages_cpp)
add_dependencies(comm_multicast iris_msgs_generate_messages_cpp)