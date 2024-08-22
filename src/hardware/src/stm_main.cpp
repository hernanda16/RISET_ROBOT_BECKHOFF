/**
 * @author Hernanda Achmad Priyatna - hernanda16
 * @brief STM Communication
*/

//--Include Headers
#include "hardware/stm_udp.hpp"

STM_UDP* stm = STM_UDP::getInstance();

int main(int argc, char** argv)
{
    ros::init(argc, argv, "stm_main");
    ros::NodeHandle nh;
    ros::MultiThreadedSpinner spinner(2);

    stm->init(&nh);

    spinner.spin();
    return 0;
}