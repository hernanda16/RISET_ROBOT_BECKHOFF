#ifndef obs_HPP
#define BALL_HPP

//--ROS Headers
#include <ros/package.h>
#include <ros/ros.h>

//--ROS Messages
#include "geometry_msgs/Pose2D.h"
#include "sensor_msgs/PointCloud.h"
#include "iris_msgs/data_robots.h"

//--C++ Headers
#include <vector>
#include <yaml-cpp/yaml.h>

//--Custom Headers
#include "boost/thread/mutex.hpp"
#include "world_model/utils.hpp"

//--Subscribers
ros::Subscriber sub_vision_obs_data;
ros::Subscriber sub_robot_pose;
ros::Subscriber sub_shared_data;

//--Publishers
ros::Publisher pub_obstacle;
ros::Publisher pub_obstacle_pcl;
ros::Publisher pub_obstacle_shared_all;

//--Timer
ros::Timer tim_routine;

//--Prototypes
void callbackRoutine(const ros::TimerEvent &event);
void callbackSubcribeVisionDataObs(const sensor_msgs::PointCloudConstPtr &msg);
void callbackSubcribeRobotPose(const geometry_msgs::Pose2DConstPtr &msg);
void callbackSubcribeSharedData(const iris_msgs::data_robotsConstPtr &msg);

std::vector<point2d_t> fuseObs();
std::vector<point2d_t> fuseObsLocal();
std::vector<point2d_t> filterAllObs(std::vector<point2d_t> obs_local);
std::vector<point2d_t> mergeSharedObs(std::vector<point2d_t> obs_local);
void loadConfig();

//--Global Variables
std::vector<point2d_t> obs;
std::vector<point2d_t> my_obs_on_field;
std::vector<point2d_t> friends_obs_on_field;

pose2d_t robot_pose;
point2d_t all_robot_pos[5];
uint8_t all_robot_state[5];
int16_t LUTfr2lap[1152000]; // 360 * 3200 (frame)

shrobot_data_t shared_robot_data[4];

boost::mutex mtx_obs;

typedef struct
{
    int16_t min_scan_obs;
    int16_t max_scan_obs;
} config_t;

typedef struct
{
    point2d_t point_src;
    point2d_t point_dst;
    uint8_t assosiated;
    float dist_to_zero;

    uint32_t counter_hilang;
} point_2_point_associative_t;

config_t obs_config;

//---Config tmabahan
float k_thresshold = 30;
uint16_t minimum_child_points_to_be_grouped = 1;

#endif