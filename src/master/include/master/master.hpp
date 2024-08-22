#ifndef MASTER_HPP
#define MASTER_HPP

#include <ros/package.h>

//--ROS Messages
#include "geometry_msgs/Pose2D.h"
#include "iris_msgs/bs2pc.h"
#include "iris_msgs/data_robots.h"
#include "iris_msgs/goalpost.h"
#include "iris_msgs/pc2stm.h"
#include "iris_msgs/stm2pc.h"
#include "sensor_msgs/PointCloud.h"
#include "std_msgs/Float32MultiArray.h"
#include "std_msgs/Int16.h"
#include "std_msgs/UInt8.h"

//--Include
#include "master/motion.hpp"
#include "master/roles/abstract.hpp"
#include "master/roles/attacker.hpp"
#include "master/roles/defender.hpp"
#include "master/roles/goalkeeper.hpp"
#include "master/utils.hpp"

//--Other Include
#include "boost/thread/mutex.hpp"
#include <yaml-cpp/yaml.h>

#define MOTION_CONTROL_RATE 0.01
#define MASTER_CONTROL_RATE 0.01

class Master {
public:
    Master(ros::NodeHandle* nh);
    ~Master();

    //--Publisher
    ros::Publisher pub_vel;
    ros::Publisher pub_pc2stm;
    ros::Publisher pub_pose_offset;
    ros::Publisher pub_data_robot;
    ros::Publisher pub_robot_condition;
    ros::Publisher pub_robot_offset;
    ros::Publisher pub_target_intercept;
    ros::Publisher pub_pose_used;
    ros::Publisher pub_my_role;

    //--Subscriber
    ros::Subscriber sub_bs2pc;
    ros::Subscriber sub_stm2pc;
    ros::Subscriber sub_obstacle;
    ros::Subscriber sub_obstacle_pcl;
    ros::Subscriber sub_velocity;
    ros::Subscriber sub_robot_data;
    ros::Subscriber sub_final_ball;
    ros::Subscriber sub_final_ball_vel;
    ros::Subscriber sub_waypoints_ball;
    ros::Subscriber sub_final_pose;
    ros::Subscriber sub_ball_status;
    ros::Subscriber sub_ball_status_vision;
    ros::Subscriber sub_ball_status_hardware;
    ros::Subscriber sub_goalpost_data;
    ros::Subscriber sub_positioning;

    //--Timer
    ros::Timer tim_routine;

    //--Prototypes
    void callbackRoutine(const ros::TimerEvent& event);
    void callbackSubscribeBs2pc(const iris_msgs::bs2pcConstPtr& msg);
    void callbackSubscribeStm2pc(const iris_msgs::stm2pcConstPtr& msg);
    void callbackSubscribeRobotData(const iris_msgs::data_robotsConstPtr& msg);
    void callbackSubscribeFinalPose(const geometry_msgs::Pose2DConstPtr& msg);
    void callbackSubscribeFinalBall(const geometry_msgs::Pose2DConstPtr& msg);
    void callbackSubscribeFinalBallVel(const geometry_msgs::Pose2DConstPtr& msg);
    void callbackSubscribeWaypointsBall(const sensor_msgs::PointCloudConstPtr& msg);
    void callbackSubscribeObstacle(const sensor_msgs::PointCloudConstPtr& msg);
    void callbackSubscribeObstaclePCL(const sensor_msgs::PointCloudConstPtr& msg);
    void callbackSubscribeBallStatus(const std_msgs::UInt8ConstPtr& msg);
    void callbackSubscribeBallStatusVision(const std_msgs::UInt8ConstPtr& msg);
    void callbackSubscribeBallStatusHardware(const std_msgs::UInt8ConstPtr& msg);
    void callbackSubscribeVelocity(const geometry_msgs::Pose2DConstPtr& msg);
    void callbackSubscribeGoalpostData(const iris_msgs::goalpostConstPtr& msg);
    void callbackSubscribePositioning(const std_msgs::Float32MultiArrayConstPtr& msg);

    //--Prototypes
    void transmitAll();
    void keyboardHandler();
    void BSCommandHandler();
    void hardwareButtonHandler();
    void batteryChecker();
    void syncMemory();
    void playSelectorControl();
    void setOffset(float pose_x, float pose_y, float pose_theta);
    void loadConfig();

    // float angleToTarget(float target_x, float target_y);

    // void manualMotion(float vx, float vy, float vtheta);
    // void worldToLocalMotion(float vx, float vy, float vtheta);

    // int8_t motionToPoint(float target_x, float target_y, float target_theta, uint16_t minmax_vel_linear, uint16_t minmax_vel_angular);
    // int8_t motionAroundPoint(float target_x, float target_y, float target_theta, uint16_t radius_point, uint16_t minmax_vel_linear, uint16_t minmax_vel_angular);
    // int8_t motionAroundPointAngle(float target_x, float target_y, int16_t radius, float arc_target_th, float robot_target_th, int16_t minmax_vel_linear, int16_t minmax_vel_angular, bool flags = 0);

    // int8_t ruckigToPoint(float target_x, float target_y, float target_theta, profile_t translation_profile, profile_t rotation_profile, uint16_t minmax_vel_linear, uint16_t minmax_vel_angular);
    // int8_t motionToPoint(float target_x, float target_y, float target_theta, profile_t translation_profile, profile_t rotation_profile);
    // int8_t positionControl(float target_x, float target_y, float target_theta, profile_t translation_profile, profile_t rotation_profile);

    // Misc
    uint8_t robot_num_bin[6] = { 0b00000, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000 };
    uint8_t robot_num = 2; // TODO : Load nomer robot sementara aku hardcode
    /**
     * @brief Robot role
     * 0 : Goalkeeper
     * 1 : Attacker
     * 2 : Defender
     */
    uint8_t role = 2;

    // Velocity actuator
    vel2d_t act_vel;
    vel2d_t world_vel;

    // Velocity feedback
    vel2d_t feedback_vel;

    // Acceleration actuator
    acc2d_t world_accel;

    // Acceleration feedback
    acc2d_t feedback_accel;

    // STM data to be sent
    pc2stm_t stm_data;

    // Obstacle
    std::vector<point2d_t> obstacle;
    std::vector<point2d_t> obstacle_pcl;

    // Predict from motion
    pose2d_t pose_used;

    // Basestation data
    bs_t bs_data;

    // Robot Shared Data
    shrobot_data_t shared_robot_data[4];

    // STM2PC data
    stm2pc_t stm2pc_data;

    // Final pose
    pose2d_t final_pose;
    pose2d_t init_offset;

    // Ball on filed
    ball_t ball;
    ball_t nearest_ball;
    std::vector<point2d_t> ball_waypoints;

    uint8_t enemy_locked;
    obstacle_checking_t enemy_locked_obs;
    float bola_terakhir_sebelum_hilang[2];
    uint8_t receive_lock;

    float region_by_style;
    float prev_ball[2] = { ball.pos.x, ball.pos.y };
    int prev_n_has_ball;

    // EMPTY GOAL STATUS
    float empty_goal_on_field[3]; // x, y,th
    float empty_goal_on_frame[3]; // x, y,th
    float min_tolerance_empty_goal_on_field[3];
    float max_tolerance_empty_goal_on_field[3];
    float target_goal_vision;
    uint8_t empty_goal_status;
    uint8_t lock_goal_kiri_atau_kanan;
    float target_goal[2];
    float keeper_on_field[2]; // x, y

    //--Class
    // Game Status
    MachineState game_status;
    MachineState game_sub_status_prep;
    MachineState game_sub_status;
    Motion* motion;

    // FSM
    MachineState fsm_level_0; // Status pertandingan [kickoff away, dropball, freekick home, dll]
    MachineState fsm_level_1; // Status sequence preparation [positioning, passing, receiving, dll]
    MachineState fsm_level_2; // Status sequence permainan [...]
    int16_t robot_condition;
    int8_t robot_base_action;
    uint8_t has_prep = 0;

    // Robot multirole
    std::unique_ptr<RoleAbstract> role_player[3]; // 3 Untuk KRI karena menggunakan 3 robot

    boost::mutex mutex_obs;
    boost::mutex mutex_waypoint_ball;

    double last_time_stm_connected;

    /// zzzz misc 2023 penting
    float pos_buffer_robot_has_ball[2];
    uint8_t hisap_dribble_lama;
    float titik_final_positioning[2];
};

#endif