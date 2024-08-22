/**
 * @author Hernanda Achmad Priyatna - hernanda16
 * @brief Motor Communication
*/

//--Include Headers
#include "hardware/motor_rs232.hpp"

Motor_RS232* motor = Motor_RS232::getInstance();

int main(int argc, char** argv)
{
    ros::init(argc, argv, "motor_main");
    ros::NodeHandle nh;
    ros::MultiThreadedSpinner spinner(0);

    motor->init(&nh);

    spinner.spin();
    return 0;
}
