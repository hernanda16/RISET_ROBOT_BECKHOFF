cmake_minimum_required(VERSION 3.0.2)
project(master)

## Compile as C++11, supported in ROS Kinetic and newer
## Ruckig requires C++17
add_compile_options(-std=c++17)

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

set(
  MASTER_SOURCES
  src/master_definition.cpp
  src/motion.cpp
  src/utils.cpp
  src/roles/attacker.cpp 
  src/roles/defender.cpp
  src/roles/goalkeeper.cpp
)

## Declare a C++ executable
add_executable(master src/master.cpp ${MASTER_SOURCES})

## Specify libraries to link a library or executable target against
target_link_libraries(master  ${catkin_LIBRARIES} ${YAML_CPP_LIBRARIES} /usr/local/lib/libruckig.so)

add_dependencies(master iris_msgs_generate_messages_cpp)
