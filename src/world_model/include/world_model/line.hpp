#ifndef LINE_HPP
#define LINE_HPP

//--ROS Headers
#include <ros/ros.h>

//--ROS Messages
#include "sensor_msgs/PointCloud.h"

//--C++ Headers
#include <vector>

//--Custom Headers
#include "world_model/utils.hpp"

//--Subscribers
ros::Subscriber sub_vision_line_data;

//--Publishers

//--Timer
ros::Timer tim_routine;

//--Prototypes
void callbackRoutine(const ros::TimerEvent& event);
void callbackSubcribeVisionDataLine(const sensor_msgs::PointCloudConstPtr& msg);

//--Global Variables
std::vector<point2d_t> line;
std::vector<point2d_t> line_in_world;

#endif