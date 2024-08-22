#ifndef LINE_HPP
#define LINE_HPP

//--ROS Headers
#include <ros/package.h>
#include <ros/ros.h>

//--ROS Messages
#include "geometry_msgs/Pose2D.h"
#include "sensor_msgs/PointCloud.h"
#include "std_msgs/Float32.h"

//--C++ Headers
#include <vector>
#include <yaml-cpp/yaml.h>

//--Custom Headers
#include "world_model/utils.hpp"

//---PCL
#include "pcl/point_types.h"
#include "pcl/registration/icp.h"

//---OpenCV
#include "opencv2/opencv.hpp"

// #define USE_NATIONAL

//--Subscribers
ros::Subscriber sub_vision_line_data;
ros::Subscriber sub_robot_pose;

//--Publishers
ros::Publisher pub_world_line;
ros::Publisher pub_d_icp;
ros::Publisher pub_icp_score;
ros::Publisher pub_line_reference;

//--Timer
ros::Timer tim_routine;

//--Prototypes
void callbackRoutine(const ros::TimerEvent &event);
void callbackSubcribeRobotPose(const geometry_msgs::Pose2DConstPtr &msg);
void callbackSubcribeVisionDataLine(const sensor_msgs::PointCloudConstPtr &msg);
void loadConfig();

// Transformer
// 1200 x 800
float cm2px_x(float x);
float cm2px_y(float y);
float px2cm_x(float x);
float px2cm_y(float y);

void create_line_ref_icp();
void create_line_ref_icp_regional();
void create_line_ref_icp_regional_real();

//--Global Variables
std::vector<point2d_t> detection_line;
std::vector<point2d_t> detection_line_in_world;
std::vector<point2d_t> reference_line_in_world;

cv::Mat icp_debugger;

pcl::PointCloud<pcl::PointXYZ> points_final_fit;

float icp_score;
float dx_icp;
float dy_icp;
float dth_icp;

pose2d_t robot_pose;
int16_t LUTfr2lap[1152000]; // 360 * 3200 (frame)

typedef struct
{
    int16_t min_scan_line;
    int16_t max_scan_line;
    int16_t min_line_points_found_threshold;
    int16_t threshold_icp_score;
    int16_t pos_diff;
    int16_t icp_max_iterations;
} config_t;

config_t line_config;

#endif