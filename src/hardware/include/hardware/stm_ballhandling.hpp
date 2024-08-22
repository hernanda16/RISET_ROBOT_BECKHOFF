#ifndef BALLHANDLING_HPP
#define BALLHANDLING_HPP

//--ROS Headers
#include <ros/package.h>
#include <ros/ros.h>

//--ROS Messages
#include "geometry_msgs/Pose2D.h"
#include "iris_msgs/pc2stm.h"
#include "iris_msgs/stm2pc.h"
#include "std_msgs/Float32MultiArray.h"
#include "std_msgs/Int16MultiArray.h"
#include "std_msgs/UInt8.h"

//--C++ Headers
#include <boost/thread/mutex.hpp>
#include <fstream>
#include <yaml-cpp/yaml.h>

/* Pre-processor functions and variables */
#define GET_FLAG(flags, flag) ((flags & flag) == flag)
#define DABS(a) ((a) >= 0 ? (a) : -(a))
// #define DEG2CNTS 1.483
#define DEG2CNTS 1.06111
#define RAD2DEG 57.295780
#define DEG2RAD 0.017452925
#define BALLROBOTDIST 30
#define DRIBBLE_RADIUS 3.2
#define BALL_RADIUS 11.6
#define MAXVOLT 24.0f // Dribble motor
#define VREF 36.0f // Dribble motor
#define MAXPWM 667 // MAXVOLT / VREF * 1000 // 667

/* System threshold */
#define DEAD_THRES 1 // in seconds

/* System flags */
#define STM_STATUS 0b000001
#define HYSTERESIS 0b000010
#define KICK 0b000100
#define SYSTEM_ENABLE 0b001000

//--Kicking flags
#define NO_AIM 0b0001
#define KICKER_POSITION_AIM 0b0010
#define THROTTLE_AIM 0b0100
#define OFFBALLWITHDRIBBLE 0b1000
#define ONLY_CHARGE 0b10000
#define ONLY_DISCHARGE 0b100000

#define SET_BIT(var, pos) ((var |= (1 << pos)))
#define CLEAR_BIT(var, pos) ((var &= ~(1 << pos)))
#define TOGGLE_BIT(var, pos) ((var ^= (1 << pos)))
#define CHECK_BIT(var, pos) (((var) & (1 << pos)))
#define COMPARE_BIT(var, flag) ((var & flag) == flag)

//--Publisher
ros::Publisher pub_to_stm;
ros::Publisher pub_to_motor;
ros::Publisher pub_state_ballhandling;
ros::Publisher pub_hysteresis_ball;
ros::Publisher pub_pwm_dribble_aux;

//--Subscriber
ros::Subscriber sub_stm2pc;
ros::Subscriber sub_pc2stm;
ros::Subscriber sub_odometry;
ros::Subscriber sub_online_calibration;
ros::Subscriber sub_final_pose;
ros::Subscriber sub_v_robot_actuation;

//--Timer
ros::Timer tim_routine;

//--Prototypes
void callbackRoutine(const ros::TimerEvent&);
void callbackSubcribeSTM2PC(const iris_msgs::stm2pcConstPtr& msg);
void callbackSubcribePC2STM(const iris_msgs::pc2stmConstPtr& msg);
void callbackSubcribeOdometry(const geometry_msgs::Pose2DConstPtr& msg);
void callbackSubcribeOnlineCalibration(const std_msgs::UInt8ConstPtr& msg);
void callbackSubscribeFinalPose(const geometry_msgs::Pose2DConstPtr& msg);
void callbackSubscribersub_v_robot_actuation(const geometry_msgs::Pose2DConstPtr& msg);

void saveConfig();
void loadConfig();
uint8_t onlineCalibration();

/**
 * @brief Control the robot to kick the ball
 * @note Order of state machine pattern
 * @param kicker_position_aim_states is 1 + (n_state * 3). Ex: 1 -> 4 -> 7 -> 13 -> ...
 * @param throttle_aim_states is 2 + (n_state * 3). Ex: 2 -> 5 -> 8 -> 14 -> ...
 * @param kicker_position_and_throttle_aim_states is 3 + (n_state * 3). Ex: 3 -> 6 -> 9 -> 15 -> ...
 * @param kick_state is on 99
 *
 */
void kick(float* throttle_norm, float* return_power, int16_t* return_pos, int16_t* return_mode, float* return_dribble);

/**
 * @brief This function is to control two dribble arm high,
 * Actuating of this control system is the dribble velocity
 * Feedback of this control system is dribble arm high (based on throttle)
 */
void throttleControl(float* data, uint8_t* sp, float* ret, float* PID_conf);
void throttleControlFloat(float* data, float* sp, float* ret, float* PID_conf);
void throttleControlNoRetract(float* data, uint8_t* sp, float* ret, float* PID_conf);

/**
 * It same as in main timer callback,
 * but this encapsulated on a function
 */
void dribbleVelocityControl(int16_t* data, float* sp, float* ret, float* PID_conf);

/**
 * @brief This function will convert a robot velocity to ball velocity,
 * Basic of odometry is just a velocity based on odometry center (rear of robot).
 * so we integral that velocity, turning into robot pose,
 * that robot pose is translated to center of robot,
 * once we have a robot pose, we derivate it and turning it to robot velocity,
 * So the final robot velocity was set.
 *
 * That final robot velocity is calculated to get an expected ball velocity
 *
 * Ball velocity will be used on kinematicsV3 function that return a set point of dribble velocity (deg/s)
 */
void encs2Local2BallVel(float* ret);

/**
 * @brief This function is to calculate feed forward kinematics,
 * The output from this function is a dribble velocity
 */
void kinematicsV3(float* ball_vel, float* ret);

void dribbleUpDownControl(float* data, uint8_t* sp, int16_t* ret);

void throttleControlNoRetract(float* data, uint8_t* sp, float* ret, float* PID_conf);

//--Global Variables

/**
 * * Tuningable Variables
 */
//=================================================================================================
typedef struct tuningable_data_tag {
    /**
     * @brief Arm throttle calibration
     */
    uint8_t perfect_backward_arm[2];
    uint8_t perfect_forward_arm[2];
    uint8_t low_dribble_arm_calibration[2];
    uint8_t high_dribble_arm_calibration[2];
    uint8_t dribble_arm_kick[2];
    uint8_t perfect_dribble_up[2];

    /**
     * @brief GP sensor threshold
     */
    uint8_t gp_sensor_threshold[2];

    uint8_t status_ball_threshold[2];

    /**
     * @brief PID control
     */
    float kp_throttle_control;
    float ki_throttle_control;
    float kd_throttle_control;
    float kPID_throttle_control[3];
    float kp_dribble_vel;
    float ki_dribble_vel;
    float kd_dribble_vel;
    float kPID_dribble_vel[3];
} tuningable_data_t;

tuningable_data_t conf;
//=================================================================================================

/**
 * * STM Data
 */
//=================================================================================================
/**
 * @brief 0: left, 1: right
 */
uint8_t curr_arm_throttle[2];

/**
 * @brief 0: left, 1: right
 */
int16_t dribble_vx[2];

/**
 * @brief 0: left, 1: right
 */
uint8_t curr_gp_value[2];
int16_t curr_kicker_position;
int16_t left_enc;
int16_t right_enc;
double last_time_stm_update;

/**
 * @brief System flags
 * @note 0: STM status, 1: Hysteresis, 2: Kick, 3: System enable
 */
uint8_t system_flags;

typedef struct
{
    uint8_t button1_pressed; // The state of button one ( lower left )
    uint8_t button2_pressed; // The state of button two ( lower right )
    uint8_t button3_pressed; // The state of button one ( higher left )
    uint8_t button4_pressed; // The state of button one ( higher mid )
    uint8_t button5_pressed; // The state of button one ( higher right )
    uint8_t toggle1_pressed; // The left toggle state
    uint8_t toggle2_pressed; // The middle toggle state
    uint8_t toggle3_pressed; // The right toggle state
    double last_time_connected;
    float battery;
} stm2pc_t;

stm2pc_t stm_data;
//=================================================================================================

/**
 * * Online Calibration
 */
//=================================================================================================
uint8_t online_calibration = 0;
uint8_t low_arm_calibration[2] = { 0, 0 };
uint8_t high_arm_calibration[2] = { 0, 0 };
uint8_t has_calibrated = 0;
//=================================================================================================

/**
 * * Kicking Variables
 */
//=================================================================================================
uint16_t kick_pos;
float kick_power;
uint8_t kick_mode;
uint16_t global_kicker_position = 800;
uint8_t dribble_up;
uint8_t enable_auto_correct_position;
uint8_t force_dribble_control;
uint8_t long_dribble_control = 0;
uint8_t stand_by = 0;

//=================================================================================================

/**
 * * Robot Variable
 */
//=================================================================================================
/**
 * @brief 0: x, 1: y, 2: theta
 */
float robot_vel_master[3];
/**
 * @brief 0: x, 1: y, 2: theta
 */
float robot_pose[3] = { 0, 0, 90 };
//=================================================================================================

/**
 * * Cascade System Control
 * @note Output from integral arm become input for integral dribble
 */
static float integral_arm[2];

//=================================================================================================
double last_timer = 0;

float v_robot[3] = { 0 };

#endif