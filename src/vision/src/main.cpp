#include "ros/ros.h"
#include "std_msgs/UInt8MultiArray.h"

void Callback(const std_msgs::UInt8MultiArray::ConstPtr& msg)
{
    //Access the data in array format
    std::vector<uint8_t>
        dataArray = msg->data;
    ROS_INFO("I heard: ");
    // Use the dataArray as needed
    for (size_t i = 0; i < dataArray.size(); i++) {
        // Access individual elements of the array
        uint8_t element = dataArray[i];
        // Do something with the element
        ROS_WARN("Element %ld: %d", i, static_cast<uint8_t>(element));
    }
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "subscriber_node");
    ros::NodeHandle nh;
    ROS_INFO("Subscriber node started");

    ros::Subscriber sub = nh.subscribe("/robot_ui/slider_data", 10, Callback);

    ros::spin();
    return 0;
}
