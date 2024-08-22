#ifndef GOALPOST_HPP
#define GOALPOST_HPP

//--ROS Headers
#include <ros/package.h>
#include <ros/ros.h>

//--ROS Messages
#include "geometry_msgs/Pose2D.h"
#include "iris_msgs/goalpost.h"
#include "std_msgs/UInt16MultiArray.h"

//--OpenCV
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>
#include <opencv2/opencv.hpp>

//--C++ Headers
#include <boost/thread/mutex.hpp>
#include <vector>
#include <yaml-cpp/yaml.h>

//--Custom Headers
#include "boost/thread/mutex.hpp"
#include "world_model/utils.hpp"

#define SET_BIT(var, pos) ((var |= (1 << pos)))
#define CLEAR_BIT(var, pos) ((var &= ~(1 << pos)))
#define TOGGLE_BIT(var, pos) ((var ^= (1 << pos)))
#define CHECK_BIT(var, pos) ((var) & (1 << pos))

#define DEG_TO_RAD *M_PI * 0.005555555555
#define DISTANCE_BETWEEN_POINTS(x1, y1, x2, y2) (sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2)))

typedef struct
{
    int frame_x;
    int frame_y;
    float angle_frame;
    float dist_frame;

    float field_x;
    float field_y;
    float angle_field;
    float dist_field;

} Point_frame_field;

//--Subscribers
ros::Subscriber sub_robot_pose;
ros::Subscriber sub_control_box;
image_transport::Subscriber sub_frame_bgr;

//--Publishers
ros::Publisher pub_goalpost;
image_transport::Publisher pub_frame_goalpost;
image_transport::Publisher pub_frame_goalpost_binary;

//--Timer
ros::Timer tim_routine;

using namespace cv;
using namespace std;

//--Global Variable
boost::mutex mutex_frame_bgr;
boost::mutex mutex_frame_bgr_copy;

const uint16_t g_res_x = 360;
const uint16_t g_res_y = 640;

//--Matrix Images
Mat frame_bgr = Mat::zeros(g_res_y, g_res_x, CV_8UC3);

/**
 * @param 0 frame_bgr
 */
uint8_t flag = 0b00000000;

/**
 * Index 0-5 is for field color threshold
 * Index 6-11 is for ball color threshold
 * Index 12-17 is for line color threshold
 * Index 18-23 is for GoalKeeper color threshold
 * Index 24-25 is for line range scanning
 * Index 30 is for field toggle switch
 * Index 31 is for ball toggle switch
 * Index 32 is for line toggle switch
 */
uint16_t controlbox_data[33];

//--Prototypes
void callbackRoutine(const ros::TimerEvent &event);
void callbackSubcribeRobotPose(const geometry_msgs::Pose2DConstPtr &msg);
void callbackSubcribeFrameBGR(const sensor_msgs::ImageConstPtr &msg);
void callbackSubControlBox(const std_msgs::UInt16MultiArray::ConstPtr &msg);

void loadConfig();

void calculateKeeperPosition(point2d_t px_fr, float real_keeper_on_field[2]);
void field2FrameScanning(Point_frame_field *point_struct, int tolerance, uint8_t *keeper_found, const Scalar &color, Mat *frame_binary_keeper_obs, Mat *frame_roi_bgr, float real_keeper_on_field[2]);
void frameFieldCaluculate(Point_frame_field *point_struct);

//--Global Variables
pose2d_t robot_pose;
int16_t LUTlap2fr[432000];  // 360 * 1200 (field)
int16_t LUTfr2lap[1152000]; // 360 * 3200 (frame)

#endif