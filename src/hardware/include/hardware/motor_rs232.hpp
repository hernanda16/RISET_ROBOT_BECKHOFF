/**
 * @author Hernanda Achmad Priyatna - hernanda16
 * @brief Handling communication with motor driver by rs232
 * * Datasheets:
 * ? https://www.pk-rus.ru/fileadmin/download/simpleiq_command_referense_manual.pdf
 * ? https://www.motorpowerco.com/media/filer_public/e6/6b/e66b3d73-815d-465d-af5a-f4379ae9d8c4/man-solgui.pdf
 */

#ifndef MOTOR_RS232_HPP
#define MOTOR_RS232_HPP

//--ROS Headers
#include "ros/package.h"
#include <ros/ros.h>

//--ROS Messages
#include "geometry_msgs/Pose2D.h"
#include "iris_msgs/stm2pc.h"
#include "std_msgs/Float32MultiArray.h"
#include "std_msgs/UInt16MultiArray.h"
#include <std_msgs/Int32MultiArray.h>

//--C++ Headers
#include <boost/thread/mutex.hpp>
#include <yaml-cpp/yaml.h>

//--Include Headers
#include "hardware/rs232.h"

#define BAUD_RATE 57600
#define DEG2RAD *0.017452925
#define RAD2DEG *57.295780
#define ENC2CM 0.000036544981
#define ENC2DEG 0.0000544623747
// #define ENC2CM 0.0000202
// #define ENC2CM_X 0.0000310 // Kacek 5
// #define ENC2CM_Y 0.0000121 // Kacek 5
// #define ENC2DEG 0.0000259707
#define CNTS2DEG (360.0f / 13186.0f)
#define DEG2CNTS (13186.0f / 360.0f)
#define RAD2CNTS (13186.0f * 0.5 / M_PI)
#define MOTOR2CENTER 22.4
#define MOTOR_RADIUS 6.3
#define AUX_CONST 1
#define DEAD_THRESH 1

#define SET_BIT(var, pos) ((var |= (1 << pos)))
#define CLEAR_BIT(var, pos) ((var &= ~(1 << pos)))
#define TOGGLE_BIT(var, pos) ((var ^= (1 << pos)))
#define CHECK_BIT(var, pos) (((var) & (1 << pos)) >> pos)

typedef struct encoder_tag {
    int32_t base_enc;
    int16_t angle;

    /* By velocity */
    float x;
    float y;
    float th;

    /* By position */
    int64_t px;
    int64_t prev_px;
    float v_fpx;

    uint8_t px_set;
    uint8_t px_set_cnt;

    /* Based on robot */
    float x_robot;
    float y_robot;
} encoder_t;

using namespace std;

class Motor_RS232 {
private:
    Motor_RS232() {};
    ~Motor_RS232() {};

    // RS232 mode
    char mode[4] = { '8', 'N', '1', 0 };

    // Motor port
    uint8_t left_motor;
    uint8_t right_motor;
    uint8_t rear_motor;

    // PID
    // ? [0] -> Proportional Constant, [1] -> Integrator Constant
    int k_pi_left[2];
    // ? [0] -> Proportional Constant, [1] -> Integrator Constant
    int k_pi_right[2];
    // ? [0] -> Proportional Constant, [1] -> Integrator Constant
    int k_pi_rear[2];

    uint8_t initialized = 0x00000000;

    /**
     * @brief Button Data
     * @note 0 -> button1_pressed
     * @note 1 -> button2_pressed
     * @note 2 -> button3_pressed
     * @note 3 -> button4_pressed
     * @note 4 -> button5_pressed
     * @note 5 -> toggle1_pressed
     * @note 6 -> toggle2_pressed
     * @note 7 -> toggle3_pressed
     */
    uint8_t buttons = 0b11111111;

public:
    static Motor_RS232*
    getInstance()
    {
        static Motor_RS232 instance_;
        return &instance_;
    }

    Motor_RS232(Motor_RS232 const&) = delete;
    void operator=(Motor_RS232 const&) = delete;

    //--Prototypes
    void loadConfig();
    void init(ros::NodeHandle* nh);
    void GetMotorEnc(unsigned char* raw, uint8_t size, encoder_t* enc_);
    void callbackRoutine(const ros::TimerEvent& event);
    void callbackSubcribeVel(const geometry_msgs::Pose2DConstPtr& msg);
    void callbackSubcribeSTM2PC(const iris_msgs::stm2pcConstPtr& msg);
    void callbackSubcribeAdditionalVelBallhandling(const std_msgs::Float32MultiArrayConstPtr& msg);

    //--Subscriber
    ros::Subscriber sub_velocity;
    ros::Subscriber sub_stm2pc;
    ros::Subscriber sub_additional_vel_ballhandling;

    ros::Publisher pub_enc_motor;

    //--Timer
    ros::Timer tim_routine;

    struct vel_tag {
        float x;
        float y;
        float theta;
    } robot_vel;

    float add_vel[2];

    int32_t enc_motor_v[3];
};

#endif