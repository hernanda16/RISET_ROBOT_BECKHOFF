/**
 * @author Hernanda Achmad Priyatna - hernanda16
 * @brief Capture image from camera and publish it to ROS topic
 */

/**
 * Command linux penting:
 * v4l2-ctl -L
 * v4l2-ctl --list-formats-ext -d [device]
 */

//--ROS Headers
#include <image_transport/image_transport.h>
#include <ros/package.h>
#include <ros/ros.h>

//--ROS Messages
#include <iris_msgs/CameraConfig.h>
#include <std_msgs/UInt16MultiArray.h>

//--OpenCV Headers
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>

//--C++ Headers
#include <yaml-cpp/yaml.h>

using namespace cv;
using namespace std;

//--Camera Params Config
uint16_t camera_params[3];

//--Prototypes
void callbackRoutine(const ros::TimerEvent& event);
void loadConfig();
void saveConfig();
void setCameraConfig(std::string camera_name, char* _param, short int _value);
bool callbackServiceCameraParams(iris_msgs::CameraConfig::Request& req, iris_msgs::CameraConfig::Response& res);
void callbackSusbcribeCameraParams(const std_msgs::UInt16MultiArray::ConstPtr& msg);

//--Publisher
image_transport::Publisher pub_frame_bgr;

//--Services
ros::ServiceServer ser_camera_params;

//--Subscriber
ros::Subscriber sub_camera_params;

//--Timer
ros::Timer tim_routine;

//--Vid Capture Path
std::string camera_path;

VideoCapture cap;

int main(int argc, char** argv)
{
    ros::init(argc, argv, "capture");

    ros::NodeHandle nh;
    ros::MultiThreadedSpinner spinner(0);
    image_transport::ImageTransport IT(nh);

    loadConfig();

    pub_frame_bgr = IT.advertise("/vision/frame_bgr", 1);

    cap.open(camera_path);

    if (!cap.isOpened()) {
        ROS_ERROR("CAPTURE ERROR : Camera cannot be opened");
        ros::shutdown();
    }

    cap.set(CAP_V4L2, 1);
    cap.set(CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(CAP_PROP_FRAME_HEIGHT, 720);
    cap.set(CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    cap.set(CAP_PROP_FPS, 60);

    setCameraConfig(camera_path, "brightness", camera_params[0]);
    setCameraConfig(camera_path, "contrast", camera_params[1]);
    setCameraConfig(camera_path, "saturation", camera_params[2]);
    setCameraConfig(camera_path, "power_line_frequency", 2);
    setCameraConfig(camera_path, "white_balance_temperature_auto", 1);
    setCameraConfig(camera_path, "exposure_auto", 3);
    setCameraConfig(camera_path, "backlight_compensation", 0);
    setCameraConfig(camera_path, "zoom_absolute", 100);

    // TAMBAHAN LOMBA
    // setCameraConfig(camera_path, "auto_exposure", 1);
    // setCameraConfig(camera_path, "exposure_time_absolute", 10);

    // DEFAULT
    // setCameraConfig(camera_path, "auto_exposure", 3);
    // setCameraConfig(camera_path, "exposure_time_absolute", 2047);

#ifndef MSI
#define MSI
#endif
#ifdef MSI
    setCameraConfig(camera_path, "exposure_dynamic_framerate", 0);
#else
    setCameraConfig(camera_path, "exposure_auto_priority", 0);
#endif

    ser_camera_params = nh.advertiseService("/camera_params_ser", callbackServiceCameraParams);
    sub_camera_params = nh.subscribe("/camera_params", 1, callbackSusbcribeCameraParams);
    tim_routine = nh.createTimer(ros::Duration(0.0166666666666), callbackRoutine);

    spinner.spin();

    return 0;
}

void callbackRoutine(const ros::TimerEvent& event)
{
    if (!cap.isOpened()) {
        ROS_ERROR("CAPTURE ERROR : Camera connection lost!");
        ros::shutdown();
    }

    //--Store the captured to matrix
    Mat vision_capture_bgr;
    cap >> vision_capture_bgr;

    //--Flip
    flip(vision_capture_bgr, vision_capture_bgr, 0); // //
    resize(vision_capture_bgr, vision_capture_bgr, Size(640, 360));
    rotate(vision_capture_bgr, vision_capture_bgr, ROTATE_90_COUNTERCLOCKWISE);

    //--Publish Field Only
    sensor_msgs::ImagePtr msg_frame_bgr = cv_bridge::CvImage(std_msgs::Header(), "bgr8", vision_capture_bgr).toImageMsg();
    pub_frame_bgr.publish(msg_frame_bgr);
}

void loadConfig()
{
    char* robot_num;
    robot_num = getenv("ROBOT");
    char config_file[100], config_file_2[100];
    std::string current_dir = ros::package::getPath("vision");
    sprintf(config_file, "%s/../../config/IRIS%s/static_conf.yaml", current_dir.c_str(), robot_num);
    sprintf(config_file_2, "%s/../../config/IRIS%s/dynamic_conf.yaml", current_dir.c_str(), robot_num);

    YAML::Node config = YAML::LoadFile(config_file);
    camera_path = config["Camera"]["camera_path"].as<std::string>();

    YAML::Node config_2 = YAML::LoadFile(config_file_2);
    camera_params[0] = config_2["Vision"]["brightness"].as<int>();
    camera_params[1] = config_2["Vision"]["contrast"].as<int>();
    camera_params[2] = config_2["Vision"]["saturation"].as<int>();
}

void saveConfig()
{
    try {
        char* robot_num = getenv("ROBOT");
        char config_file[100];
        std::string current_dir = ros::package::getPath("vision");
        sprintf(config_file, "%s/../../config/IRIS%s/dynamic_conf.yaml", current_dir.c_str(), robot_num);

        YAML::Node config = YAML::LoadFile(config_file);
        config["Vision"]["brightness"] = to_string(camera_params[0]);
        config["Vision"]["contrast"] = to_string(camera_params[1]);
        config["Vision"]["saturation"] = to_string(camera_params[2]);

        std::ofstream fout(config_file);
        fout << config;
        fout.close();
    } catch (const std::exception& e) {
        ROS_ERROR("SAVE CAMERA CONFIG %s", e.what());
    }
}

void setCameraConfig(std::string camera_name, char* _param, short int _value)
{
    char cmd[128];
    sprintf(cmd, "v4l2-ctl -d %s -c %s=%d", camera_name.c_str(), _param, _value);
    system(cmd);
}

bool callbackServiceCameraParams(iris_msgs::CameraConfig::Request& req, iris_msgs::CameraConfig::Response& res)
{
    try {
        for (uint8_t i = 0; i < 3; i++)
            res.data.push_back(camera_params[i]);
    } catch (const std::exception& e) {
        ROS_ERROR("CAMERA PARAMS %s", e.what());
    }
    return true;
}

void callbackSusbcribeCameraParams(const std_msgs::UInt16MultiArray::ConstPtr& msg)
{
    memcpy(camera_params, &msg->data[0], 6);

    setCameraConfig(camera_path, "brightness", camera_params[0]);
    setCameraConfig(camera_path, "contrast", camera_params[1]);
    setCameraConfig(camera_path, "saturation", camera_params[2]);

    saveConfig();
}