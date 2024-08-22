cmake_minimum_required(VERSION 3.0.2)
project(hardware)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  roslib
  std_msgs
  geometry_msgs ##temp
  iris_msgs
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

set(MOTOR_LIB
  src/motor_rs232.cpp
  src/rs232.c
)

set(STM_LIB
  src/stm_udp.cpp
)

## Declare a C++ executable
add_executable(motor_main src/motor_main.cpp ${MOTOR_LIB})
add_executable(stm_main src/stm_main.cpp ${STM_LIB})
add_executable(stm_ballhandling src/stm_ballhandling.cpp)   #${STM_LIB}     
add_executable(stm_serial src/stm_serial.cpp)
add_executable(stm_udp_2023 src/stm_udp_2023.cpp)
add_executable(beckhoff_ec src/beckhoff_ec.cpp)

## Specify libraries to link a library or executable target against
target_link_libraries(motor_main ${catkin_LIBRARIES} ${YAML_CPP_LIBRARIES})
target_link_libraries(stm_main ${catkin_LIBRARIES})
target_link_libraries(stm_ballhandling ${catkin_LIBRARIES} ${YAML_CPP_LIBRARIES})
target_link_libraries(stm_serial ${catkin_LIBRARIES})
target_link_libraries(stm_udp_2023 ${catkin_LIBRARIES})
target_link_libraries(beckhoff_ec ${catkin_LIBRARIES} soem)

add_dependencies(stm_main iris_msgs_generate_messages_cpp)
add_dependencies(stm_ballhandling iris_msgs_generate_messages_cpp)