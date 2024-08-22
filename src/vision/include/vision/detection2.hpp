/**
 * @author: Hernanda Achmad Priyatna - hernanda16
 * @brief: Detection of ball, line, and obstacles
 *
 */

//--ROS Headers
#include <image_transport/image_transport.h>
#include <ros/package.h>
#include <ros/ros.h>

//--ROS Messages
#include "iris_msgs/CameraConfig.h"
#include "iris_msgs/vision_status.h"
#include "sensor_msgs/PointCloud.h"
#include "std_msgs/UInt16MultiArray.h"

//--OpenCV Headers
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>

//--C++ Headers
#include <algorithm>
#include <boost/thread/mutex.hpp>
#include <yaml-cpp/yaml.h>

//--Include Headers
#include "vision/utils.hpp"

#define DEG2RAD *0.017452925
#define RAD2DEG *57.295780

using namespace std;
using namespace cv;

//--Timer
ros::Timer tim_routine;

//--Subscriber
ros::Subscriber sub_control_box;
ros::Subscriber sub_goalpost;
image_transport::Subscriber sub_frame_bgr;

//--Publisher
ros::Publisher pub_vision_ball_data;
ros::Publisher pub_vision_line_data;
ros::Publisher pub_vision_obs_data;
ros::Publisher pub_vision_status;
image_transport::Publisher pub_frame_display;
image_transport::Publisher pub_frame_yuv;
image_transport::Publisher pub_frame_ball;
image_transport::Publisher pub_frame_ball_debug;
image_transport::Publisher pub_frame_line_debug;
image_transport::Publisher pub_frame_line_pcl_debug;
image_transport::Publisher pub_frame_obs_debug;
image_transport::Publisher pub_frame_field_debug;

//--Prototypes ROS
void callbackRoutine(const ros::TimerEvent &event);
void callbackSubcribeFrameBGR(const sensor_msgs::ImageConstPtr &msg);
void callbackSubControlBox(const std_msgs::UInt16MultiArray::ConstPtr &msg);

//--Prototypes
void findBallCenter(vector<Point> *center_ptr, Mat mask);
void findPointCloud(vector<Point> *line_ptr, Mat mask, uint8_t is_rising = 1);
void loadConfig();
void saveConfig();

//--Global Variables
uint16_t center_cam_x;
uint16_t center_cam_y;
uint16_t cam_res_x = 360;
uint16_t cam_res_y = 640;
uint16_t outer_mask_radius;
uint16_t inner_mask_radius;

//--Matrix Images
Mat frame_bgr; // BGR for buffer data from capture

Mat frame_bgr_processed; // BGR for processing data
Mat frame_hsv_processed; // HSV for processing data
Mat frame_yuv_processed; // YUV for processing data
Mat frame_hls_processed; // HLS for processing data

//----Field
Mat frame_field_displayed;
Mat frame_field_hull;
Mat threshold_field = Mat::zeros(Size(cam_res_x, cam_res_y), CV_8UC1);

//----Goalkeeper
// In World Model -> Goalpost

//----Ball
Mat frame_ball_displayed;
Mat threshold_ball;

//----Line
Mat frame_bgr_displayed;
Mat threshold_line = Mat::zeros(Size(cam_res_x, cam_res_y), CV_8UC1);
Mat frame_line_pcl = Mat::zeros(Size(cam_res_x, cam_res_y), CV_8UC1);

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

/**
 * @param 0 frame_bgr
 */
uint8_t flag = 0b00000000;

boost::mutex mutex_frame_bgr;