#ifndef STM_SERIAL_HPP
#define STM_SERIAL_HPP

//--ROS Headers
#include <ros/ros.h>

//--ROS Messages
#include <std_msgs/Int16MultiArray.h>

//--C++ Headers
#include <stdio.h>
#include <string.h>

//--Linux Headers
#include <errno.h> // Error integer and strerror() function
#include <fcntl.h> // Contains file controls like O_RDWR
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

//--Publisher
ros::Publisher pub_motor_enc;

//--Subscriber
ros::Subscriber sub_motor_pwm;

//--Prototypes
void callbackMotorPwm(const std_msgs::Int16MultiArray::ConstPtr& msg);

//--Prototypes
int8_t stmOpenSerial();

//--Global Variables
int serial_port_fd;

#endif