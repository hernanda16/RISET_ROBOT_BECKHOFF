#ifndef BALL_HPP
#define BALL_HPP

//--ROS Headers
#include <ros/ros.h>

//--ROS Messages
#include "geometry_msgs/Pose2D.h"
#include "iris_msgs/data_robots.h"
#include "sensor_msgs/PointCloud.h"
#include "std_msgs/UInt8.h"

//--C++ Headers
#include "boost/thread/mutex.hpp"
#include "world_model/kf_1d_v_const.h"
#include "world_model/utils.hpp"
#include <vector>

#include "world_model/pandu_kf.h"

#define BALL_STATUS_NOT_SEEN 0
#define BALL_STATUS_OPPONENT 1
#define BALL_STATUS_SELF 2
#define BALL_STATUS_FRIEND 3

//--Subscribers
ros::Subscriber sub_vision_ball_data;
ros::Subscriber sub_robot_pose;
ros::Subscriber sub_obs_on_field;
ros::Subscriber sub_shared_data;

//--Publishers
ros::Publisher pub_ball_final_pos;
ros::Publisher pub_ball_final_vel;
ros::Publisher pub_ball_status;
ros::Publisher pub_ball_waypoints;

//--Timer
ros::Timer tim_routine;

//--Prototypes
void callbackRoutine(const ros::TimerEvent& event);
void callbackSubcribeVisionDataBall(const sensor_msgs::PointCloudConstPtr& msg);
void callbackSubcribeRobotPose(const geometry_msgs::Pose2DConstPtr& msg);
void callbackSubcribeObsOnField(const sensor_msgs::PointCloudConstPtr& msg);
void callbackSubcribeSharedData(const iris_msgs::data_robotsConstPtr& msg);
shrobot_data_t shared_robot_data[4];

point2d_t fuseBalls();
point2d_t fuseBalls_2023();
void checkBallOnOpponent(point2d_t ball_on_field);
std::vector<point2d_t> createBallWaypoints(differential_v_const_t ball_x, differential_v_const_t ball_y, float time_max_s = 10, float time_step_s = 0.1);
void init_kf_vars();

//--Global Variables
std::vector<point2d_t> ball;
std::vector<balls_vote_t> my_ball_in_world;
std::vector<balls_vote_t> friends_ball_in_world;
std::vector<point2d_t> obs_on_field;

float bola_theta = 0;

pose2d_t robot_pose;
pose2d_t prev_robot_pose;
pose2d_t robot_vel;
int16_t LUTfr2lap[1152000]; // 360 * 3200 (frame)
boost::mutex mtx_ball;

// KalmanFilter kf_ball;
// int n = 4; // Number of states
// int m = 2; // Number of measurements
// double t = 0; // Time
// double dt = 0.02; // Time step
// Eigen::MatrixXd A(n, n); // System dynamics matrix
// Eigen::MatrixXd C(m, n); // Output matrix
// Eigen::MatrixXd Q(n, n); // Process noise covariance
// Eigen::MatrixXd R(m, m); // Measurement noise covariance
// Eigen::MatrixXd P(n, n); // Estimate error covariance

KalmanFilter kf_ball;
int n = 4; // Number of states
int m = 2; // Number of measurements
double t = 0; // Time
double dt = 0.02; // Time step
Eigen::MatrixXd A(n, n); // System dynamics matrix
Eigen::MatrixXd C(m, n); // Output matrix
Eigen::MatrixXd Q(n, n); // Process noise covariance
Eigen::MatrixXd R(m, m); // Measurement noise covariance
Eigen::MatrixXd P(n, n); // Estimate error covariance

KF kf_final_ball_x;
KF kf_final_ball_y;

double first_time_opponent_has_ball = 0;

/**
 * 0 Tidak lihat bola
 * 1 Bola dipegang lawan
 * 2 Lihat bola dari saya
 * 3 Lihat bola dari teman
 */
uint8_t ball_status;
uint8_t prev_ball_status;

//----Config
float opponent_ball_distance_threshold = 80;
float nearest_ball_threshold = 80;

// TITIP
float ball_distance_frame = 0;
float ball_angle_frame = 0;
float ball_frame_x = 0;
float ball_frame_y = 0;

#endif