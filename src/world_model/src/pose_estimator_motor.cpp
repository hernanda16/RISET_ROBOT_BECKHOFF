// //--ROS Headers
// #include <ros/package.h>
// #include <ros/ros.h>

// //--ROS Messages
// #include "geometry_msgs/Pose2D.h"
// #include <std_msgs/Int32MultiArray.h>

// //--C++ Headers
// #include "world_model/utils.hpp"
// #include <yaml-cpp/yaml.h>

// #define ENC2CM 0.0000202
// #define ENC2CM_X 0.0000310 // Kacek 5
// #define ENC2CM_Y 0.0000121 // Kacek 5
// #define ENC2DEG 0.0000259707

// ros::Subscriber sub_enc_motor;

// float motor_enc_v[3]; // left, right, rear

// void callback_sub_enc_motor(const std_msgs::Int32MultiArrayConstPtr &msg);

// int main(int argc, char **argv)
// {
//     ros::init(argc, argv, "pose_estimator_motor");
//     ros::NodeHandle nh;
//     ros::MultiThreadedSpinner spinner(4);

//     return 0;
// }

// void callback_sub_enc_motor(const std_msgs::Int32MultiArrayConstPtr &msg)
// {
//     motor_enc_v[0] = msg->data.at(0);
//     motor_enc_v[1] = msg->data.at(1);
//     motor_enc_v[2] = msg->data.at(2);
// }
