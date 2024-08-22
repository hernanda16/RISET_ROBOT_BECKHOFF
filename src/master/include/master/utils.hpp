#ifndef UTILS_HPP
#define UTILS_HPP

#include "math.h"
#include "stdint.h"
#include "stdio.h"
#include "sys/ioctl.h"
#include "termios.h"
#include <sys/time.h>
#include <unistd.h>

#include "master/enums.hpp"
#include "master/pid.h"
#include "master/simple_fsm.h"

#include "string"
#include <stdarg.h>

//--ROS Header
#include <ros/ros.h>

#define DEG2RAD *0.017452925
#define RAD2DEG *57.295780

//--Kicking flags
#define NO_AIM 0b0001
#define KICKER_POSITION_AIM 0b0010
#define THROTTLE_AIM 0b0100
#define OFFBALLWITHDRIBBLE 0b1000
#define ONLY_CHARGE 0b10000
#define ONLY_DISCHARGE 0b100000
#define SET_POSITION 0b000000

#define FIELD_Y_0 0
#define FIELD_X_0 0
#define FIELD_Y_1 1200
#define FIELD_X_1 800

#define FIELD_Y_1_2 (FIELD_Y_1 / 2.f)
#define FIELD_X_1_2 (FIELD_X_1 / 2.f)

#define FIELD_Y_1_3 (FIELD_Y_1 / 3.f)
#define FIELD_X_1_3 (FIELD_X_1 / 3.f)

#define FIELD_Y_1_4 (FIELD_Y_1 / 4.f)
#define FIELD_X_1_4 (FIELD_X_1 / 4.f)

#define FIELD_Y_1_5 (FIELD_Y_1 / 5.f)
#define FIELD_X_1_5 (FIELD_X_1 / 5.f)

#define FIELD_Y_2_3 (FIELD_Y_1 * 2.f / 3.f)
#define FIELD_X_2_3 (FIELD_X_1 * 2.f / 3.f)

#define FIELD_Y_3_4 (FIELD_Y_1 * 3.f / 4.f)
#define FIELD_X_3_4 (FIELD_X_1 * 3.f / 4.f)

#define FIELD_X_4_5 (FIELD_X_1 * 4.f / 5.f)
#define FIELD_Y_4_5 (FIELD_Y_1 * 4.f / 5.f)

#define NORMAL 0b000000
#define INVERT 0b01
#define NORMAL_OBSTACLE 0b10
#define NORMAL_OBSTACLE_BALL 0b100
#define USE_OBS_MERGE 0b1000
#define INVERT_OBS 0b10000
#define NORMAL_OBSTACLE_FRIEND 0b100000
#define PREDICT 0b1000000
#define LANGSUNG_AMBIL 0b10000000

typedef struct
{
    uint8_t kicker_mode;
    float kicker_power;
    uint16_t kicker_position;
    uint8_t buzzer_cnt;
    uint8_t buzzer_time;
    uint16_t global_kicker_position = 800;
    uint8_t dribble_up;
    uint8_t enable_auto_correct_position = 1;
    uint8_t force_dribble_control = 0;
    uint8_t long_dribble_control;
    uint8_t stand_by = 0;

} pc2stm_t;

typedef struct
{
    uint8_t status;
    float angle;
    float distance;
    float pos_x;
    float pos_y;
} obstacle_checking_t;

typedef struct
{
    uint8_t status;
    float angle;
    float distance;
    float vx;
    float vy;
} avoid_obs_t;

typedef struct
{
    uint8_t is_iam_controlled_by_bs;
    uint8_t is_iam_send_data_to_bs;
    uint8_t command;
    uint8_t mode;
    uint8_t manual_mode;
    uint8_t auto_calibration;
    uint8_t style;
    uint16_t data_mux1;
    uint16_t data_mux2;
    uint16_t mux_control;
    int16_t obs_bs_regional[3];
    double last_time_connected;
    int8_t trim_vel_linear;
    int8_t trim_vel_angular;
    int8_t trim_kick_power;

    uint8_t n_active_robot; // How many active robots?
    uint8_t n_nearest_ball; // Who has closest to ball?
    uint8_t n_has_ball;     // Who has ball?
    uint8_t n_pass;         // Who is passing?
    uint8_t n_pass_target;  // Who is receiver of passing?

    uint8_t n_robot_att;    // Number robot of attacker role
    uint8_t n_robot_assist; // Number robot of assist role
    uint8_t n_robot_gk;     // Number robot of Goalkeeper role
    uint8_t n_robot_def_1;  // Number robot of Defender1 role
    uint8_t n_robot_def_2;  // Number robot of Defender2 role

    uint8_t prev_style;
} bs_t;

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

typedef struct
{
    float x;
    float y;
} point2d_t;

typedef struct
{
    float r;
    float theta;
} polar2d_t;

typedef struct
{
    float x;
    float y;
    float theta;
} pose2d_t;

typedef pose2d_t vel2d_t;
typedef vel2d_t acc2d_t;

typedef struct
{
    point2d_t pos;
    uint8_t is_visible;
    uint8_t is_caught;
    vel2d_t vel;
    /**
     * 0 Tidak lihat bola
     * 1 Bola dipegang lawan
     * 2 Lihat bola dari saya
     * 3 Lihat bola dari teman
     */
    uint8_t status;

    float distance;
    float angle;
} ball_t;

typedef struct
{
    uint8_t role;
    pose2d_t pose;
    int16_t robot_condition;
    int16_t target_positioning[2];
} shrobot_data_t;

typedef struct
{
    float max_velocity;
    float max_acceleration;
    float max_jerk;
} profile_t;

typedef struct target_goal_tag
{
    float goal_tengah[2];
    float goal_kiri[2];
    float goal_kanan[2];
    float status_gawang_kosong;
} target_goal_t;

// // EMPTY GOAL STATUS
// float empty_goal_on_field[3]; // x, y,th
// float empty_goal_on_frame[3]; // x, y,th
// float min_tolerance_empty_goal_on_field[3];
// float max_tolerance_empty_goal_on_field[3];
// uint8_t empty_goal_status;
// float target_goal[2];

//--Prototype
uint64_t getTimeNowUSec();
uint64_t getTimeNowMSec();
uint64_t getTimeNowSec();
float pythagoras(point2d_t point1, point2d_t point2);
float pythagoras(float x0, float y0, float x1, float y1);
int16_t kbhit();
void readU16Bit(uint8_t *dst, const uint16_t *src, uint8_t total_bit, uint8_t offset_bit);
float angleToTarget(float target_x, float target_y, pose2d_t final_pose);
void kickerControl(pc2stm_t *stm_data, uint8_t mode, float power = 0, uint16_t position = 800);
void buzzerControl(pc2stm_t *stm_data, uint8_t cnt, uint8_t time);
float deltaAngle(float angle1, float angle2, bool absolute = true);
void mirrorPosX(float *pos_x);

float robotAngletoOwnGoal(pose2d_t final_pose);
float robotAngletoEnemyGoal(pose2d_t final_pose, uint8_t shoot_the_ball = 0);

/**
 * @brief Calculate the attractive force
 * @param q1 current position x
 * @param qd1 target position x
 * @param q2 current position y
 * @param qd2 target position y
 * @param Katt attractive force
 * @param linear_slope
 * @param rad_thr
 * @param U_trans normally 100
 * @param grad_q1 output velocity x
 * @param grad_q2 output velocity y
 * @param a normally 1
 * @param b normally 1
 * @param c normally 0.03
 */
void AttractiveForce(float q1, float qd1, float q2, float qd2, double Katt,
                     double linear_slope, double rad_thr, double U_trans, float &grad_q1, float &grad_q2, double a = 1, double b = 1, double c = 0.001);

void LogWithColor(std::string color, const char *text, ...);

#endif