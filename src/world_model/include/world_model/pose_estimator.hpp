#ifndef POSE_ESTIMATOR_HPP
#define POSE_ESTIMATOR_HPP

// #define ENC2CM *0.01250445
#define ENC2CM *0.01241445663
// #define ENC2CM *0.0133169091

// #define ENC2CM *0.012542555

//--ROS Headers
#include <ros/package.h>
#include <ros/ros.h>

//--ROS Messages
#include "geometry_msgs/Pose2D.h"
#include "iris_msgs/stm2pc.h"

//--C++ Headers
#include "world_model/utils.hpp"
#include <yaml-cpp/yaml.h>

//--Subscribers
ros::Subscriber sub_stm2pc;
ros::Subscriber sub_d_icp;
ros::Subscriber sub_pose_offset;

//--Publishers
ros::Publisher pub_pose;
ros::Publisher pub_vel;
ros::Publisher pub_icp_target_offset;
ros::Publisher pub_velocity_local;

//--Timer
ros::Timer tim_routine;

//--Prototypes
void callbackRoutine(const ros::TimerEvent& event);
void callbackSubcribeSTM2PC(const iris_msgs::stm2pcConstPtr& msg);
void callbackSubscribeDICP(const geometry_msgs::Pose2DConstPtr& msg);
void callbackSubscribePoseOffset(const geometry_msgs::Pose2DConstPtr& msg);
void setOffset(float x, float y, float theta);
void setOffset(pose2d_t offset_pose);
void loadConfig();

//--Global Variables
pose2d_t final_pose;
pose2d_t robot_vel;
pose2d_t robot_vel_local;

enc_t left_enc;
enc_t right_enc;
gyro_t gyro;

geometry_msgs::Pose2D pose_msg;
geometry_msgs::Pose2D vel_msg;

pose2d_t init_offset;

typedef struct
{
    float icp_gain;
    point2d_t mechanical_offset; // Ketidaktepatan center odometry dengan center robot
    point2d_t mechanical_offset_gain;
} config_t;

config_t estimator_config;

#endif