#include "world_model/line.hpp"

int main(int argc, char **argv)
{
    ros::init(argc, argv, "line_node");
    ros::NodeHandle nh;
    ros::MultiThreadedSpinner spinner(2);

    sub_vision_line_data = nh.subscribe("/vision/data/line", 1, callbackSubcribeVisionDataLine);
    tim_routine = nh.createTimer(ros::Duration(0.02), callbackRoutine);

    spinner.spin();
    return 0;
}

void callbackRoutine(const ros::TimerEvent &event)
{
    sensor_msgs::PointCloud line_world_msg;

    for (uint16_t i = 0; i < line.size(); i++)
    {
        point2d_t line_world_point;
        // cam2world(line[i], &line_world_point);

        geometry_msgs::Point32 temp;
        temp.x = line_world_point.x;
        temp.y = line_world_point.y;
        temp.z = 0;

        line_world_msg.points.push_back(temp);
    }

    // Publish
    // pub_world_line.publish(line_world_msg);
}

void callbackSubcribeVisionDataLine(const sensor_msgs::PointCloudConstPtr &msg)
{
    line.clear();
    size_t total_line_points = msg->points.size();
    for (uint16_t i = 0; i < total_line_points; i++)
    {
        point2d_t temp;
        temp.x = msg->points[i].x;
        temp.y = msg->points[i].y;

        line.push_back(temp);
    }
}