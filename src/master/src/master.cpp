#include "master/master.hpp"

int main(int argc, char** argv)
{
    ros::init(argc, argv, "master");
    ros::NodeHandle nh;
    ros::MultiThreadedSpinner spinner(0);

    Master master(&nh);

    spinner.spin();
    return 0;
}