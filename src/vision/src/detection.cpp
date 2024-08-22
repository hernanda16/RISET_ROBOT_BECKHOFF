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

#define SET_BIT(var, pos) ((var |= (1 << pos)))
#define CLEAR_BIT(var, pos) ((var &= ~(1 << pos)))
#define TOGGLE_BIT(var, pos) ((var ^= (1 << pos)))
#define CHECK_BIT(var, pos) ((var) & (1 << pos))

#define DEG_TO_RAD *M_PI * 0.005555555555
#define DISTANCE_BETWEEN_POINTS(x1, y1, x2, y2) (sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2)))

uint16_t center_cam[2];

#define OUTER_R 268
#define INNER_R 60

const uint16_t g_res_x = 360;
const uint16_t g_res_y = 640;

using namespace std;
using namespace cv;

//--Timer
ros::Timer tim_routine;

//--Subscriber
image_transport::Subscriber sub_frame_bgr;
ros::Subscriber sub_control_box;
ros::Subscriber sub_goalpost;

//--Publisher
ros::Publisher pub_vision_ball_data;
ros::Publisher pub_vision_line_data;
ros::Publisher pub_vision_obs_data;
ros::Publisher pub_vision_status;
image_transport::Publisher pub_frame_display;
image_transport::Publisher pub_frame_display_ball;
image_transport::Publisher pub_frame_yuv;
image_transport::Publisher pub_field_debug;
image_transport::Publisher pub_frame_ball_debug;
image_transport::Publisher pub_frame_line_debug;
image_transport::Publisher pub_frame_line_pcl_debug;
image_transport::Publisher pub_frame_obs_debug;

//--Prototypes
void callbackRoutine(const ros::TimerEvent &event);
void callbackSubcribeFrameBGR(const sensor_msgs::ImageConstPtr &msg);
void callbackSubControlBox(const std_msgs::UInt16MultiArray::ConstPtr &msg);
void callbackSubGoalPost(const std_msgs::UInt16MultiArray::ConstPtr &msg);
void findBallCenter(vector<Point> *center_ptr, Mat mask);
void findPointCloud(vector<Point> *line_ptr, Mat mask);
void findPointClouds(vector<vector<Point>> *lines_ptr, Mat mask);
void findBlob(vector<Point> *blob_ptr, Mat mask);
void loadConfig();
void saveConfig();

//--Matrix Images
Mat frame_bgr;

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
} point_frame_field_t;

//--Global Variable
boost::mutex mutex_frame_bgr;
boost::mutex mutex_frame_bgr_copy;
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

/**
 * Index 0 is for left goalpost x
 * Index 1 is for left goalpost y
 * Index 2 is for center goalpost x
 * Index 3 is for center goalpost y
 * Index 4 is for right goalpost x
 * Index 5 is for right goalpost y
 */
uint16_t goalpost_data[6];

iris_msgs::vision_status vision_status_msg;
sensor_msgs::PointCloud vision_ball_msg;
sensor_msgs::PointCloud vision_line_msg;
sensor_msgs::PointCloud vision_obs_msg;

int main(int argc, char **argv)
{
    ros::init(argc, argv, "detection_node");
    ros::NodeHandle nh;
    ros::MultiThreadedSpinner spinner(2);

    loadConfig();

    image_transport::ImageTransport IT(nh);
    pub_vision_ball_data = nh.advertise<sensor_msgs::PointCloud>("/vision/data/ball", 1);
    pub_vision_line_data = nh.advertise<sensor_msgs::PointCloud>("/vision/data/line", 1);
    pub_vision_obs_data = nh.advertise<sensor_msgs::PointCloud>("/vision/data/obs", 1);
    pub_vision_status = nh.advertise<iris_msgs::vision_status>("/vision/status", 1);
    pub_frame_display = IT.advertise("/vision/display", 1);
    pub_frame_display_ball = IT.advertise("/vision/display/ball", 1);
    pub_frame_yuv = IT.advertise("/vision/display/yuv", 1);
    pub_field_debug = IT.advertise("/vision/debug/field", 1);
    pub_frame_ball_debug = IT.advertise("/vision/debug/ball", 1);
    pub_frame_line_debug = IT.advertise("/vision/debug/line", 1);
    pub_frame_line_pcl_debug = IT.advertise("/vision/debug/line_pcl", 1);
    pub_frame_obs_debug = IT.advertise("/vision/debug/obs", 1);
    sub_frame_bgr = IT.subscribe("/vision/frame_bgr", 1, callbackSubcribeFrameBGR);
    sub_goalpost = nh.subscribe("/world_model/goalpost", 1, callbackSubGoalPost);
    sub_control_box = nh.subscribe("/threshold_params", 1, callbackSubControlBox);
    tim_routine = nh.createTimer(ros::Duration(0.008333333), callbackRoutine);

    spinner.spin();

    return 0;
}

void callbackSubControlBox(const std_msgs::UInt16MultiArray::ConstPtr &msg)
{
    /**
     * This is callback from web_adapter
     * Index 0 is data identifier
     * Index 1 is data lenght
     * Index 2 and more is the data
     *
     * For the datas, see controlbox_data variable
     */
    memcpy(controlbox_data + msg->data[0] * 6, &msg->data[0] + 2, msg->data[1] * sizeof(uint16_t));
}

void callbackSubGoalPost(const std_msgs::UInt16MultiArray::ConstPtr &msg)
{
    goalpost_data[0] = msg->data[0];
    goalpost_data[1] = msg->data[1];
    goalpost_data[2] = msg->data[2];
    goalpost_data[3] = msg->data[3];
    goalpost_data[4] = msg->data[4];
    goalpost_data[5] = msg->data[5];
}

void callbackRoutine(const ros::TimerEvent &event)
{
    if (flag == 0)
    {
        return;
    }

    mutex_frame_bgr_copy.lock();
    Mat frame_bgr_copy = frame_bgr.clone();
    CLEAR_BIT(flag, 0);
    mutex_frame_bgr_copy.unlock();

    //===================================================================================

    Mat frame_yuv;
    Mat frame_yuv_display;
    Mat frame_hls_line;
    Mat frame_hsv;
    cvtColor(frame_bgr_copy, frame_yuv, CV_BGR2YUV);
    cvtColor(frame_bgr_copy, frame_hls_line, CV_BGR2HLS);
    cvtColor(frame_bgr_copy, frame_hsv, COLOR_BGR2HSV);
    frame_yuv_display = frame_yuv.clone();

    Mat threshold_ball;
    inRange(frame_yuv, Scalar(controlbox_data[6], controlbox_data[8], controlbox_data[10]), Scalar(controlbox_data[7], controlbox_data[9], controlbox_data[11]), threshold_ball);

    Mat threshold_line;
    inRange(frame_hls_line, Scalar(controlbox_data[12], controlbox_data[14], controlbox_data[16]), Scalar(controlbox_data[13], controlbox_data[15], controlbox_data[17]), threshold_line);
    // ROS_INFO("HLS: %d %d %d %d %d %d", controlbox_data[12], controlbox_data[13], controlbox_data[14], controlbox_data[15], controlbox_data[16], controlbox_data[17]);

    Mat threshold_field;
    blur(frame_hsv, frame_hsv, Size(3, 3));
    inRange(frame_hsv, Scalar(controlbox_data[0], controlbox_data[2], controlbox_data[4]), Scalar(controlbox_data[1], controlbox_data[3], controlbox_data[5]), threshold_field);
    // dilate(threshold_field, threshold_field, getStructuringElement(MORPH_ELLIPSE, Size(20, 20)));
    // erode(threshold_field, threshold_field, getStructuringElement(MORPH_ELLIPSE, Size(14, 14)));
    // dilate(threshold_field, threshold_field, getStructuringElement(MORPH_ELLIPSE, Size(10, 10)));

    Mat threshold_obs_final;
    bitwise_or(threshold_field, threshold_line, threshold_obs_final);
    bitwise_or(threshold_obs_final, threshold_ball, threshold_obs_final);
    // bitwise_not(threshold_obs_final, threshold_obs_final);

    vector<vector<Point>> contour_field; // list of contour points
    vector<Vec4i> hierarchy_field;
    findContours(threshold_field, contour_field, hierarchy_field, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

    // create hull array for convex hull points
    vector<vector<Point>> hull(contour_field.size());
    for (int i = 0; i < contour_field.size(); i++)
        convexHull(Mat(contour_field[i]), hull[i], false);

    Mat hull_field = Mat::zeros(frame_bgr_copy.size(), CV_8UC1);
    for (int i = 0; i < contour_field.size(); i++)
    {
        drawContours(hull_field, hull, i, Scalar(255), -1, 8, vector<Vec4i>(), 0, Point());
    }

    Mat drawing = Mat::zeros(frame_bgr_copy.size(), CV_8UC1);
    int largest_area = 0;
    int largest_contour_index = 0;

    for (int i = 0; i < contour_field.size(); i++)
    {
        if (contour_field[i].size() > largest_area)
        {
            largest_area = contour_field[i].size();
            largest_contour_index = i;
        }
        Scalar color_contours = Scalar(0, 255, 0); // green - color for contours
        Scalar color = Scalar(255, 0, 0);          // blue - color for convex hull
        // draw ith contour
        // drawContours(drawing, contour_field, i, color_contours, 1, 8, vector<Vec4i>(), 0, Point());
        // draw ith convex hull
        // drawContours(drawing, hull, i, color, 1, 8, vector<Vec4i>(), 0, Point());
    }

    drawContours(drawing, hull, largest_contour_index, Scalar(255), -1, 8, vector<Vec4i>(), 0, Point());

    bitwise_not(threshold_obs_final, threshold_obs_final);
    bitwise_and(threshold_obs_final, hull_field, threshold_obs_final);

    // if (!drawing.empty())
    // imshow("Field", drawing);

    // bitwise_and(threshold_ball, drawing, threshold_ball);
    // bitwise_and(threshold_line, drawing, threshold_line);
    // bitwise_and(threshold_obs_final, drawing, threshold_obs_final);

    //===================================================================================

    // Mat threshold_ball_inverse;
    // bitwise_not(threshold_ball, threshold_ball_inverse);
    // erode(threshold_ball_inverse, threshold_ball_inverse, getStructuringElement(MORPH_ELLIPSE, Size(6, 6)));
    // bitwise_and(threshold_obs_final, threshold_ball_inverse, threshold_obs_final);

    // Mat threshold_line_inverse;
    // bitwise_not(threshold_line, threshold_line_inverse);
    // bitwise_and(threshold_obs_final, threshold_line_inverse, threshold_obs_final);

    // DIAMETER
    Mat camera_mask = Mat::zeros(frame_bgr_copy.size(), CV_8UC1);
    circle(camera_mask, Point(center_cam[0], center_cam[1]), OUTER_R, Scalar(255), FILLED, LINE_8);
    circle(camera_mask, Point(center_cam[0], center_cam[1]), INNER_R, Scalar(0), FILLED, LINE_8);

    bitwise_and(camera_mask, threshold_ball, threshold_ball);
    bitwise_and(camera_mask, hull_field, hull_field);
    bitwise_and(camera_mask, threshold_line, threshold_line);
    bitwise_and(camera_mask, threshold_obs_final, threshold_obs_final);

    dilate(hull_field, hull_field, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)));
    bitwise_and(hull_field, threshold_ball, threshold_ball);

    erode(threshold_ball, threshold_ball, getStructuringElement(MORPH_ELLIPSE, Size(4, 4)));
    dilate(threshold_ball, threshold_ball, getStructuringElement(MORPH_ELLIPSE, Size(4, 4)));
    erode(threshold_obs_final, threshold_obs_final, getStructuringElement(MORPH_ELLIPSE, Size(4, 4)));
    dilate(threshold_obs_final, threshold_obs_final, getStructuringElement(MORPH_ELLIPSE, Size(4, 4)));

    //===================================================================================

    vision_ball_msg.points.clear();

    vector<Point> center_balls;
    findBallCenter(&center_balls, threshold_ball);

    static float bola_x_buffer;
    static float bola_y_buffer;

    vision_ball_msg.points.reserve(center_balls.size());
    vision_ball_msg.points.resize(center_balls.size());

    for (size_t i = 0; i < center_balls.size(); i++)
    {
        vision_ball_msg.points[i].x = center_balls[i].x;
        vision_ball_msg.points[i].y = center_balls[i].y;
        circle(frame_yuv, center_balls[i], 30, Scalar(0, 0, 255), 3, LINE_8);
    }

    static uint64_t delay_in = 0;
    static uint64_t delay_out = 0;

    if (center_balls.size() > 0)
    {
        bola_x_buffer = center_balls[0].x;
        bola_y_buffer = center_balls[0].y;

        delay_in++;
        delay_out = 0;
    }
    else
    {
        delay_in = 0;
        delay_out++;
    }

    if (delay_in > 1)
    {
        vision_status_msg.is_ball_seen = true;
    }

    if (delay_out > 25)
    {
        vision_status_msg.is_ball_seen = false;
    }
    else
    {
        vision_status_msg.is_ball_seen = true;

        vision_ball_msg.points.reserve(1);
        vision_ball_msg.points.resize(1);

        vision_ball_msg.points[0].x = bola_x_buffer;
        vision_ball_msg.points[0].y = bola_y_buffer;
    }

    //===================================================================================

    vision_line_msg.points.clear();

    vector<Point> pcl_line;
    findPointCloud(&pcl_line, threshold_line);

    vision_line_msg.points.reserve(pcl_line.size());
    vision_line_msg.points.resize(pcl_line.size());

    for (uint16_t i = 0; i < pcl_line.size(); i++)
    {
        vision_line_msg.points[i].x = pcl_line[i].x;
        vision_line_msg.points[i].y = pcl_line[i].y;
        circle(frame_hls_line, Point(pcl_line[i].x + center_cam[0], center_cam[1] - pcl_line[i].y), 2, Scalar(255, 0, 0), 2, LINE_8);
    }

    Mat pcl_line_frame = Mat::zeros(frame_bgr_copy.size(), CV_8UC1);
    for (uint16_t i = 0; i < pcl_line.size(); i++)
    {
        circle(pcl_line_frame, Point(pcl_line[i].x + center_cam[0], center_cam[1] - pcl_line[i].y), 2, Scalar(255), 2, LINE_8);
    }

    // vector<vector<Point>> pcl_lines;
    // ROS_INFO("SIZE: %d", pcl_lines.size());
    // findPointClouds(&pcl_lines, threshold_line);
    // for (uint16_t i = 0; i < pcl_lines.size(); i++) {
    //     for (uint16_t j = 0; j < pcl_lines[i].size(); j++) {
    //         circle(frame_hls_line, pcl_lines[i][j], 2, Scalar(50 * i, 0, 0), 2, LINE_8);
    //     }
    // }

    //===================================================================================

    vision_obs_msg.points.clear();

    bitwise_and(hull_field, threshold_obs_final, threshold_obs_final); // TODO rapihkan program

    vector<Point> pcl_obs;
    findPointCloud(&pcl_obs, threshold_obs_final);

    vision_obs_msg.points.reserve(pcl_obs.size());
    vision_obs_msg.points.resize(pcl_obs.size());

    for (uint16_t i = 0; i < pcl_obs.size(); i++)
    {
        vision_obs_msg.points[i].x = pcl_obs[i].x;
        vision_obs_msg.points[i].y = pcl_obs[i].y;
        circle(frame_yuv, Point(pcl_obs[i].x + center_cam[0], center_cam[1] - pcl_obs[i].y), 2, Scalar(0, 255, 0), 2, LINE_8);
    }

    pub_vision_ball_data.publish(vision_ball_msg);
    pub_vision_status.publish(vision_status_msg);
    pub_vision_line_data.publish(vision_line_msg);
    pub_vision_obs_data.publish(vision_obs_msg);

    // imshow("Bola & Obs", frame_yuv);
    // imshow("Line", frame_hls_line);
    // imshow("Thresh Bola", threshold_ball);
    // imshow("Thresh Line", threshold_line);
    // imshow("Thresh Obs", threshold_obs_final);

    // waitKey(1);

    //--Publish
    static uint8_t cnt = 0;

    if (cnt >= 6)
    {
        Mat frame_bgr_display = frame_bgr_copy.clone();
        Mat frame_bgr_display_ball = frame_bgr_copy.clone();
        Mat frame_threshold_ball = Mat::zeros(threshold_ball.size(), CV_8UC3);

        cvtColor(threshold_ball, frame_threshold_ball, CV_GRAY2BGR);

        if (center_balls.size() > 0)
            circle(frame_bgr_display, Point(center_balls[0].x + center_cam[0], center_cam[1] - center_balls[0].y), 5, Scalar(255, 0, 0), -1);

        // make rectangle in display with for goalpost data left and right
        // rectangle(frame_bgr_display, Point(goalpost_data[0], goalpost_data[1]), Point(goalpost_data[0] + 10, goalpost_data[1] + 10), Scalar(0, 255, 0), -1);
        line(frame_bgr_display, Point(goalpost_data[0], 400), Point(goalpost_data[2], 400), Scalar(0, 255, 0), 2);

        addWeighted(frame_bgr_display_ball, 1, frame_threshold_ball, 0.2, 0, frame_bgr_display_ball);

        // make cross line in display with center_cam[0] and center_cam[1]
        line(frame_bgr_display, Point(center_cam[0], 0), Point(center_cam[0], 720), Scalar(255, 0, 255), 1, LINE_8);
        line(frame_bgr_display, Point(0, center_cam[1]), Point(1280, center_cam[1]), Scalar(255, 0, 255), 1, LINE_8);
        circle(frame_bgr_display, Point(center_cam[0], center_cam[1]), OUTER_R, Scalar(255, 0, 255), 1, LINE_8);
        circle(frame_bgr_display, Point(center_cam[0], center_cam[1]), INNER_R, Scalar(255, 0, 255), 1, LINE_8);

#define REGRESSION
#ifdef REGRESSION
        Mat line_regression = Mat::zeros(frame_bgr_display.size(), CV_8UC3);
        for (float angle = 0; angle < 360; angle += 10)
        {
            int16_t pixel_x = center_cam[0] + 180 * cos(angle DEG_TO_RAD);
            int16_t pixel_y = center_cam[1] - 180 * sin(angle DEG_TO_RAD);
            line(line_regression, Point(center_cam[0], center_cam[1]), Point(pixel_x, pixel_y), Scalar(255, 0, 255, 255), 1, LINE_8);
        }
        addWeighted(frame_bgr_display, 1, line_regression, 0.8, 0, frame_bgr_display);
#endif
        Mat pcl_obs_frame = Mat::zeros(frame_bgr_copy.size(), CV_8UC3);
        for (uint16_t i = 0; i < pcl_obs.size(); i++)
        {
            circle(pcl_obs_frame, Point(pcl_obs[i].x + center_cam[0], center_cam[1] - pcl_obs[i].y), 2, Scalar(0, 0, 255), 2, LINE_8);
        }
        addWeighted(frame_bgr_display, 1, pcl_obs_frame, 1, 0, frame_bgr_display);

        sensor_msgs::ImagePtr msg_pub_frame_display = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame_bgr_display).toImageMsg();
        pub_frame_display.publish(msg_pub_frame_display);
        sensor_msgs::ImagePtr msg_pub_frame_yuv = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame_yuv_display).toImageMsg();
        pub_frame_yuv.publish(msg_pub_frame_yuv);
        sensor_msgs::ImagePtr msg_pub_frame_display_ball = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame_bgr_display_ball).toImageMsg();
        pub_frame_display_ball.publish(msg_pub_frame_display_ball);
        sensor_msgs::ImagePtr msg_pub_frame_ball_debug = cv_bridge::CvImage(std_msgs::Header(), "mono8", threshold_ball).toImageMsg();
        pub_frame_ball_debug.publish(msg_pub_frame_ball_debug);
        sensor_msgs::ImagePtr msg_pub_frame_line_debug = cv_bridge::CvImage(std_msgs::Header(), "mono8", threshold_line).toImageMsg();
        pub_frame_line_debug.publish(msg_pub_frame_line_debug);
        sensor_msgs::ImagePtr msg_pub_frame_line_pcl_debug = cv_bridge::CvImage(std_msgs::Header(), "mono8", pcl_line_frame).toImageMsg();
        pub_frame_line_pcl_debug.publish(msg_pub_frame_line_pcl_debug);
        sensor_msgs::ImagePtr msg_pub_frame_obs_debug = cv_bridge::CvImage(std_msgs::Header(), "mono8", hull_field).toImageMsg();
        pub_frame_obs_debug.publish(msg_pub_frame_obs_debug);
        sensor_msgs::ImagePtr msg_pub_field_debug = cv_bridge::CvImage(std_msgs::Header(), "mono8", threshold_field).toImageMsg();
        pub_field_debug.publish(msg_pub_field_debug);
        cnt = 0;
    }

    cnt++;
}

void callbackSubcribeFrameBGR(const sensor_msgs::ImageConstPtr &msg)
{
    mutex_frame_bgr.lock();
    try
    {
        frame_bgr = cv_bridge::toCvShare(msg, "bgr8")->image.clone();
        SET_BIT(flag, 0);
    }
    catch (cv_bridge::Exception &e)
    {
        ROS_ERROR("cv_bridge exception: %s", e.what());
    }
    mutex_frame_bgr.unlock();
}

void findBallCenter(vector<Point> *center_ptr, Mat mask)
{
    center_ptr->clear();
    vector<vector<Point>> contours;
    findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    std::sort(contours.begin(), contours.end(), [](const auto &c1, const auto &c2)
              {
                  double area1 = cv::contourArea(c1);
                  double area2 = cv::contourArea(c2);
                  return area1 > area2; // Sort in descending order
              });

    for (size_t i = 0; i < contours.size(); i++)
    {
        Point2f center;
        float radius;
        minEnclosingCircle(contours[i], center, radius);
        Moments m = moments(contours[i]);
        // Point center_point(m.m10 / m.m00 - center_cam[0], center_cam[1] - m.m01 / m.m00);

        // Tambahan
        if (m.m00 == 0)
            continue;
        float center_ball_x = m.m10 / m.m00;
        float center_ball_y = m.m01 / m.m00;

        Point new_center;
        new_center.x = center_ball_x; // Untuk safety
        new_center.y = center_ball_y; // Untuk safety

        int dx = center_cam[0] - center_ball_x;
        int dy = center_cam[1] - center_ball_y;
        float angle_to_center = atan2(dy, dx) * 180 / M_PI;
        float distance_to_center = sqrt(dx * dx + dy * dy);

        // Scan dari center bola ke center robot untuk mencari titik terluar bola yang paling dekat dengan robot
        for (int pixel = 0; pixel < distance_to_center; pixel++)
        {
            int x = center.x + pixel * cos(angle_to_center DEG_TO_RAD);
            int y = center.y + pixel * sin(angle_to_center DEG_TO_RAD);
            if (mask.at<uint8_t>(y, x) == 0)
            {
                new_center.x = x;
                new_center.y = y;
                break;
            }
        }

        // Mengembalikan agar titik pusat bola berada di tengah
        new_center.x = new_center.x - center_cam[0];
        new_center.y = center_cam[1] - new_center.y;

        if (i < 5)
            center_ptr->push_back(new_center);
    }
}

void findPointCloud(vector<Point> *line_ptr, Mat mask)
{
    Mat frame = Mat::zeros(mask.size(), CV_8UC1);
    Mat combine = Mat::zeros(mask.size(), CV_8UC1);
    Mat mask_clone = mask.clone();
    uint8_t pixel_value = 0;

    uint16_t last_pixel_checked[2] = {center_cam[0], center_cam[1]};

    line_ptr->clear();
    for (float angle = 0; angle < 360; angle += 2.5)
    {
        for (float magnitude = 0; magnitude < mask_clone.cols * 0.6; magnitude += 1)
        {
            uint16_t pixel_x = center_cam[0] + magnitude * cos(angle DEG_TO_RAD);
            uint16_t pixel_y = center_cam[1] - magnitude * sin(angle DEG_TO_RAD);

            //--Check if pixel is out of bounds
            if (pixel_x > mask_clone.cols || pixel_x <= 0 || pixel_y > mask_clone.rows || pixel_y <= 0)
            {
                break;
            }

            //--Check if pixel is already checked
            if (pixel_x == last_pixel_checked[0] && pixel_y == last_pixel_checked[1])
            {
                continue;
            }
            else
            {
                last_pixel_checked[0] = pixel_x;
                last_pixel_checked[1] = pixel_y;
            }

            pixel_value = mask_clone.at<uint8_t>(pixel_y, pixel_x);

            //--Check if pixel is black or white
            if (pixel_value == 0)
            {
                continue;
            }
            else if (pixel_value == 255)
            {
                line_ptr->push_back(Point(pixel_x - center_cam[0], center_cam[1] - pixel_y));
                break;
            }
        }
    }
}

void findPointClouds(vector<vector<Point>> *lines_ptr, Mat mask)
{
    Mat frame = Mat::zeros(mask.size(), CV_8UC1);
    Mat combine = Mat::zeros(mask.size(), CV_8UC1);
    Mat mask_clone = mask.clone();
    uint8_t pixel_value = 0;
    uint32_t total_lines = 0;

    uint16_t last_pixel_checked[2] = {center_cam[0], center_cam[1]};
    uint16_t pixel_white[2] = {0, 0};
    uint16_t last_pixel_white[2] = {0, 0};

    lines_ptr->clear();
    lines_ptr->resize(total_lines + 1);
    lines_ptr->reserve(total_lines + 1);

    for (float angle = 0; angle < 360; angle += 1)
    {
        for (float magnitude = 0; magnitude < mask_clone.cols * 0.6; magnitude += 2.5)
        {
            uint16_t pixel_x = center_cam[0] + magnitude * cos(angle DEG_TO_RAD);
            uint16_t pixel_y = center_cam[1] - magnitude * sin(angle DEG_TO_RAD);

            //--Check if pixel is out of bounds
            if (pixel_x > mask_clone.cols || pixel_x <= 0 || pixel_y > mask_clone.rows || pixel_y <= 0)
            {
                break;
            }

            //--Check if pixel is already checked
            if (pixel_x == last_pixel_checked[0] && pixel_y == last_pixel_checked[1])
            {
                continue;
            }
            else
            {
                last_pixel_checked[0] = pixel_x;
                last_pixel_checked[1] = pixel_y;
            }

            pixel_value = mask_clone.at<uint8_t>(pixel_y, pixel_x);

            //--Check if pixel is black or white
            if (pixel_value == 0)
            {
                continue;
            }
            else if (pixel_value == 255)
            {
                pixel_white[0] = pixel_x;
                pixel_white[1] = pixel_y;

                //--Separate lines
                if (DISTANCE_BETWEEN_POINTS(pixel_white[0], pixel_white[1], last_pixel_white[0], last_pixel_white[1]) > 4)
                {
                    if (total_lines > 0)
                        total_lines++;
                }

                last_pixel_white[0] = pixel_white[0];
                last_pixel_white[1] = pixel_white[1];

                lines_ptr->at(total_lines).push_back(Point(pixel_x - center_cam[0], center_cam[1] - pixel_y));
                break;
            }
        }
    }
}

void findBlob(vector<Point> *blob_ptr, Mat mask)
{
    Mat temp_frame = Mat::zeros(mask.size(), CV_8UC1);
    Mat combine = Mat::zeros(mask.size(), CV_8UC1);

    if (CHECK_BIT(flag, 1))
    {
        Mat mask_clone = mask.clone();
        CLEAR_BIT(flag, 1);
        ROS_WARN("FLAG CLEARED");
    }

    blob_ptr->clear();
    for (float angle = 0; angle < 360; angle += 1)
    {
        for (uint16_t magnitude = 0; magnitude < mask.cols * 0.6; magnitude++)
        {
            int16_t pixel_x = center_cam[0] + magnitude * cos(angle DEG_TO_RAD);
            int16_t pixel_y = center_cam[1] - magnitude * sin(angle DEG_TO_RAD);

            if (pixel_x > mask.cols || pixel_x < 0 || pixel_y > mask.rows || pixel_y < 0)
            {
                break;
            }

            circle(temp_frame, Point(pixel_x, pixel_y), 1, Scalar(255), FILLED, LINE_8);
        }

        vector<Point> nonzero;
        findNonZero(combine, nonzero);

        for (uint16_t i = 0; i < nonzero.size(); i++)
        {
            blob_ptr->push_back(nonzero.at(i));
            circle(temp_frame, nonzero.at(i), 1, Scalar(0), FILLED, LINE_8);
        }

        bitwise_and(temp_frame, mask, combine);

        if (angle == 359)
        {
            SET_BIT(flag, 1);
            ROS_ERROR("FLAG SET");
        }

        // if (!temp_frame.empty())
        // imshow("PCL", temp_frame);

        // imshow("mask_func", combine);
    }
}

void loadConfig()
{
    char *robot_num;
    robot_num = getenv("ROBOT");
    char config_file[100];
    char config_file1[100];
    std::string current_dir = ros::package::getPath("vision");
    sprintf(config_file, "%s/../../config/IRIS%s/dynamic_conf.yaml", current_dir.c_str(), robot_num);
    sprintf(config_file1, "%s/../../config/IRIS%s/static_conf.yaml", current_dir.c_str(), robot_num);

    YAML::Node config = YAML::LoadFile(config_file);
    controlbox_data[0] = config["Vision"]["field_h_min"].as<int>();
    controlbox_data[1] = config["Vision"]["field_h_max"].as<int>();
    controlbox_data[2] = config["Vision"]["field_s_min"].as<int>();
    controlbox_data[3] = config["Vision"]["field_s_max"].as<int>();
    controlbox_data[4] = config["Vision"]["field_v_min"].as<int>();
    controlbox_data[5] = config["Vision"]["field_v_max"].as<int>();
    controlbox_data[6] = config["Vision"]["ball_y_min"].as<int>();
    controlbox_data[7] = config["Vision"]["ball_y_max"].as<int>();
    controlbox_data[8] = config["Vision"]["ball_u_min"].as<int>();
    controlbox_data[9] = config["Vision"]["ball_u_max"].as<int>();
    controlbox_data[10] = config["Vision"]["ball_v_min"].as<int>();
    controlbox_data[11] = config["Vision"]["ball_v_max"].as<int>();
    controlbox_data[12] = config["Vision"]["line_h_min"].as<int>();
    controlbox_data[13] = config["Vision"]["line_h_max"].as<int>();
    controlbox_data[14] = config["Vision"]["line_l_min"].as<int>();
    controlbox_data[15] = config["Vision"]["line_l_max"].as<int>();
    controlbox_data[16] = config["Vision"]["line_s_min"].as<int>();
    controlbox_data[17] = config["Vision"]["line_s_max"].as<int>();

    YAML::Node config1 = YAML::LoadFile(config_file1);

    center_cam[0] = config1["Camera"]["cam_offset_x"].as<int>();
    center_cam[1] = config1["Camera"]["cam_offset_y"].as<int>();

    center_cam[0] += (int)(g_res_x * 0.5);
    center_cam[1] += (int)(g_res_y * 0.5);
}

void saveConfig()
{
    try
    {
        char *robot_num = getenv("ROBOT");
        char config_file[100];
        std::string current_dir = ros::package::getPath("vision");
        sprintf(config_file, "%s/../../config/IRIS%s/dynamic_conf.yaml", current_dir.c_str(), robot_num);

        YAML::Node config = YAML::LoadFile(config_file);
        // config["Vision"]["field_h_min"] = to_string(controlbox_data[0]);
        // config["Vision"]["field_h_max"] = to_string(controlbox_data[1]);
        // config["Vision"]["field_s_min"] = to_string(controlbox_data[2]);
        // config["Vision"]["field_s_max"] = to_string(controlbox_data[3]);
        // config["Vision"]["field_v_min"] = to_string(controlbox_data[4]);
        // config["Vision"]["field_v_max"] = to_string(controlbox_data[5]);
        // config["Vision"]["ball_y_min"] = to_string(controlbox_data[6]);
        // config["Vision"]["ball_y_max"] = to_string(controlbox_data[7]);
        // config["Vision"]["ball_u_min"] = to_string(controlbox_data[8]);
        // config["Vision"]["ball_u_max"] = to_string(controlbox_data[9]);
        // config["Vision"]["ball_v_min"] = to_string(controlbox_data[10]);
        // config["Vision"]["ball_v_max"] = to_string(controlbox_data[11]);
        // config["Vision"]["line_h_min"] = to_string(controlbox_data[12]);
        // config["Vision"]["line_h_max"] = to_string(controlbox_data[13]);
        // config["Vision"]["line_l_min"] = to_string(controlbox_data[14]);
        // config["Vision"]["line_l_max"] = to_string(controlbox_data[15]);
        // config["Vision"]["line_s_min"] = to_string(controlbox_data[16]);
        // config["Vision"]["line_s_max"] = to_string(controlbox_data[17]);

        std::ofstream fout(config_file);
        fout << config;
        fout.close();
    }
    catch (const std::exception &e)
    {
        ROS_ERROR("SAVE CAMERA CONFIG %s", e.what());
    }
}