cmake_minimum_required(VERSION 3.0.2)
project(world_model)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++14)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  iris_msgs
  roslib
  image_transport
  cv_bridge
)

find_package(yaml-cpp REQUIRED)
find_package(PCL 1.7 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

add_definitions(${PCL_DEFINITIONS})

## INCLUDE_DIRS: uncomment this if your package contains header files
catkin_package(
 INCLUDE_DIRS include
)

## Specify additional locations of header files
include_directories(
include
  ${catkin_INCLUDE_DIRS}
  ${PCL_INCLUDE_DIRS}
)

## Declare a C++ executable
add_executable(ball_node src/ball.cpp)
add_executable(line_node src/line_icp.cpp)
add_executable(obs_node src/obs.cpp)
add_executable(goalpost_node src/goalpost.cpp)
add_executable(pose_estimator src/pose_estimator.cpp)
add_executable(positioning_2023 src/positioning_2023.cpp)
add_executable(positioning_2024 src/positioning_2024.cpp)

## Specify libraries to link a library or executable target against
target_link_libraries(ball_node ${catkin_LIBRARIES})
target_link_libraries(line_node ${catkin_LIBRARIES} ${PCL_LIBRARIES} ${OpenCV_LIBS} ${YAML_CPP_LIBRARIES})
target_link_libraries(obs_node ${catkin_LIBRARIES} ${YAML_CPP_LIBRARIES})
target_link_libraries(goalpost_node ${catkin_LIBRARIES} ${YAML_CPP_LIBRARIES})
target_link_libraries(pose_estimator ${catkin_LIBRARIES} ${YAML_CPP_LIBRARIES})
target_link_libraries(positioning_2023 ${catkin_LIBRARIES} ${OpenCV_LIBS})
target_link_libraries(positioning_2024 ${catkin_LIBRARIES} ${OpenCV_LIBS})


add_dependencies(pose_estimator iris_msgs_generate_messages_cpp)
add_dependencies(positioning_2023 iris_msgs_generate_messages_cpp)
add_dependencies(positioning_2024 iris_msgs_generate_messages_cpp)
add_dependencies(goalpost_node iris_msgs_generate_messages_cpp)
add_dependencies(obs_node iris_msgs_generate_messages_cpp)

