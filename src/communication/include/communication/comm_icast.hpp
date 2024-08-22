#ifndef COMM_ICAST_HPP
#define COMM_ICAST_HPP

//--ROS Headers
#include <ros/package.h>
#include <ros/ros.h>

//--ROS Messages
#include "geometry_msgs/Pose2D.h"
#include "iris_msgs/bs2pc.h"
#include "iris_msgs/data_robots.h"
#include "iris_msgs/stm2pc.h"
#include "sensor_msgs/PointCloud.h"
#include "std_msgs/Float32MultiArray.h"
#include "std_msgs/Int16.h"
#include "std_msgs/UInt8.h"

//--Libraries
#include <icast.hpp>

//--Publisher
ros::Publisher pub_bs2pc;
ros::Publisher pub_data_robot;

//--Subscriber
ros::Subscriber sub_final_pose;
ros::Subscriber sub_stm2pc;
ros::Subscriber sub_velocity;
ros::Subscriber sub_robot_condition;
ros::Subscriber sub_final_ball;
ros::Subscriber sub_ball_status_vision;
ros::Subscriber sub_ball_status_hardware;
ros::Subscriber sub_target_intercept;
ros::Subscriber sub_obs_pcl;
ros::Subscriber sub_my_obs;
ros::Subscriber sub_positioning;

//--Timer
ros::Timer tim_routine;
ros::Timer tim_send;
ros::Timer tim_recv;

//--Protoypes
void callbackRoutine(const ros::TimerEvent& event);
void callbackSend(const ros::TimerEvent& event);
void callbackRecv(const ros::TimerEvent& event);
void callbackSubscribeFinalPose(const geometry_msgs::Pose2DConstPtr& msg);
void callbackSubscribeStm2pc(const iris_msgs::stm2pcConstPtr& msg);
void callbackSubscribeVelocity(const geometry_msgs::Pose2DConstPtr& msg);
void callbackSubscribeRobotCondition(const std_msgs::Int16ConstPtr& msg);
void callbackSubscribeFinalBall(const geometry_msgs::Pose2DConstPtr& msg);
void callbackSubscribeBallStatusVision(const std_msgs::UInt8ConstPtr& msg);
void callbackSubscribeBallStatusHardware(const std_msgs::UInt8ConstPtr& msg);
void callbackSubscribeTargetIntercept(const geometry_msgs::Pose2DConstPtr& msg);
void callbackSubscribeMyObs(const sensor_msgs::PointCloudConstPtr& msg);
void callbackSubscribePositioning(const std_msgs::Float32MultiArrayConstPtr& msg);
// void callbackSubscriberObsPcl(const )

void transmitAll();

//--Singleton Initialization
Icast* icast = Icast::getInstance();

epoch_t epoch;

typedef struct
{
    int16_t pose_x[6];
    int16_t pose_y[6];
    int16_t pose_theta[6];

    uint8_t ball_is_caught[6];
    uint8_t ball_is_visible[6];
    int16_t ball_pos_x_1[6];
    int16_t ball_pos_y_1[6];
    int16_t ball_pos_x_2[6];
    int16_t ball_pos_y_2[6];
    int16_t ball_pos_x_3[6];
    int16_t ball_pos_y_3[6];

    int16_t robot_condition[6];
    int16_t fsm_0[6];
    int16_t fsm_1[6];
    int16_t fsm_2[6];

    int16_t obs_pcl_x_1[6];
    int16_t obs_pcl_y_1[6];
    int16_t obs_pcl_x_2[6];
    int16_t obs_pcl_y_2[6];
    int16_t obs_pcl_x_3[6];
    int16_t obs_pcl_y_3[6];
} shared_data_t;

shared_data_t shared_robot_data;
ball_t ball;

uint8_t send_to_bs;

#endif