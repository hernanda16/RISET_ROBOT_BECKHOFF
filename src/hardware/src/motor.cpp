/**
 * @author Hernanda Achmad Priyatna - hernanda16
 * @brief Motor Communication
*/

//--Include Headers
#include "hardware/motor_rs232.hpp"

int main(int argc, char** argv)
{
    ros::init(argc, argv, "motor_main");

    Motor_RS232& motor = Motor_RS232::getInstance();
    motor.init();

    ros::spin();
    return 0;
}