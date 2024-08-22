/**
 * @note Setting Camera using v4l2-ctl --list-formats-ext -d [device part]
 * @note View Parameters using v4l2-ctl -d [device part] -l
*/

#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <ros/ros.h>

void SetCameraConfig(std::string camera_name, char* _param, short int _value)
{
    char cmd[128];
    sprintf(cmd, "v4l2-ctl -d %s -c %s=%d", camera_name.c_str(), _param, _value);
    system(cmd);
}

int main(int argc, char** argv)
{
    // Initialize the ROS node
    ros::init(argc, argv, "capture_node");

    // Create a ROS node handle
    ros::NodeHandle nh;
    image_transport::ImageTransport it(nh);
    image_transport::Publisher pub = it.advertise("camera/image", 1);

    cv::VideoCapture video;
    cv::VideoCapture();

    video.open(camera_path);

    cap.set(CAP_V4L2, 1);
    cap.set(CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(CAP_PROP_FRAME_HEIGHT, 720);
    cap.set(CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    cap.set(CAP_PROP_FPS, 60);

    LoadCameraConfig(camera_path, "brightness", camera_params[0]);
    LoadCameraConfig(camera_path, "contrast", camera_params[1]);
    LoadCameraConfig(camera_path, "saturation", camera_params[2]);
    LoadCameraConfig(camera_path, "power_line_frequency", 1);
    LoadCameraConfig(camera_path, "white_balance_temperature_auto", 1);
    LoadCameraConfig(camera_path, "exposure_auto", 3);
    LoadCameraConfig(camera_path, "exposure_auto_priority", 0);
    LoadCameraConfig(camera_path, "backlight_compensation", 1);
    LoadCameraConfig(camera_path, "zoom_absolute", 100);
    cv::Mat frame;

    if (video.isOpened() == false) {
        ROS_ERROR("Camera is not opened");
        return -1;
    }

    while (ros::ok()) {
        video >> frame;

        if (frame.empty()) {
            ROS_ERROR("Frame is empty");
            break;
        }

        pub.publish(cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame).toImageMsg());

        // cv::imshow("frame", frame);
        cv::waitKey(1);
    }

    // Spin the ROS node
    ros::spin();

    return 0;
}
