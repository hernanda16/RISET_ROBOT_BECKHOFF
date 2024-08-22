#ifndef MOTION_HPP
#define MOTION_HPP

//--C++ headers
#include <queue>
#include <stdint.h>

//--Include headers
#include "boost/thread/mutex.hpp"
#include "master/utils.hpp"

#include "ruckig/ruckig.hpp"

#include <ros/package.h>
#include <yaml-cpp/yaml.h>

class Motion {
private:
    Motion() { }
    pose2d_t* final_pose;
    vel2d_t* act_vel;
    vel2d_t* world_vel;
    ball_t* ball;
    pc2stm_t* stm_data;
    std::vector<point2d_t>* waypoints_ball;
    std::vector<point2d_t>* obstacle;
    std::vector<point2d_t>* obstacle_pcl;

    uint8_t use_acceleration_constant = 0;

    profile_t translation_profile_;
    profile_t translation_profile_with_ball;

    profile_t angular_profile_;
    profile_t angular_profile_with_ball;

    uint16_t center_cam[2];

    const uint16_t g_res_x = 360;
    const uint16_t g_res_y = 640;

public:
    float target_intercept_ball_x;
    float target_intercept_ball_y;

    // Target goal 2023
    uint8_t* empty_goal_status;
    float* target_goal[2];
    float* min_tolerance_empty_goal_on_field[2];
    float* max_tolerance_empty_goal_on_field[2];
    float* target_goal_vision;
    uint8_t* lock_goal_kiri_atau_kanan;

    boost::mutex mutex_obs_merged;
    boost::mutex mutex_obs_pcl;
    boost::mutex mutex_ball_waypoints;

    shrobot_data_t* shared_robot_data[4];

    vel2d_t* feedback_vel;
    pose2d_t* pose_used;

public:
    static Motion* getInstance()
    {
        static Motion instance_;
        return &instance_;
    }

    void operator=(const Motion&) = delete;
    Motion(Motion& other) = delete;

    /**
     * @brief Initialize motion
     * @param final_pose Final pose
     * @param act_vel Actual velocity
     */
    void init(pose2d_t* final_pose, vel2d_t* act_vel, vel2d_t* world_vel, ball_t* ball, pc2stm_t* stm_data, std::vector<point2d_t>* waypoints_ball, std::vector<point2d_t>* obstacle, std::vector<point2d_t>* obstacle_pcl);

    /**
     * @brief Manual motion control
     * @param vel_x Linear velocity in x-axis
     * @param vel_y Linear velocity in y-axis
     * @param vel_theta Angular velocity
     */
    void manualMotion(float vel_x, float vel_y, float vel_theta, bool is_ruckig = 0);
    /**
     * @brief Motion to a point
     * @param target_x Target x position
     * @param target_y Target y position
     * @param target_theta Target angle
     * @param minmax_vel_linear Min-max linear velocity
     * @param minmax_vel_angular Min-max angular velocity
     * @return 1 if finished, 0 if not finished
     */
    int8_t motionToPoint(float target_x, float target_y, float target_theta, uint16_t minmax_vel_linear, uint16_t minmax_vel_angular, uint8_t flag = 0);

    int8_t newMotionToPoint(float target_x, float target_y, float target_theta, uint16_t minmax_vel_linear, uint16_t minmax_vel_angular);
    /**
     * @brief Motion to a point with obstacle avoidance using Potential Field Method
     * @ref https://people.csail.mit.edu/lpk/mars/temizer_2001/Potential_Field_Method/index.html
     * @note Using pre-processor function
     * @param target_x Target x position
     * @param target_y Target y position
     * @param target_theta Target angle
     * @param minmax_vel_linear Min-max linear velocity
     * @param minmax_vel_angular Min-max angular velocity
     * @param flags Ball method for making valley
     * @return 1 if finished, 0 if not finished
     */
    int8_t motionToPointAvoidance(float target_x, float target_y, float target_theta, uint16_t minmax_vel_linear, uint16_t minmax_vel_angular, uint8_t flags = normal_obstacle_ball);

    /**
     * @brief Motion generator from current pose to target pose in arc of circle within certain radius
     * @param target_x Target x in arc of circle
     * @param target_y Target y in arc of circle
     * @param target_theta Target angle in arc of circle
     * @param robot_target_th Target angle of robot
     * @param radius_point Radius of point
     * @param minmax_vel_linear Min-max linear velocity for safety in PID
     * @param minmax_vel_angular Min-max angular velocity for safety in PID
     * @param flag Invert flag for robot facing (inwart = 0 or outward = 1)
     * @return 1 if finished, 0 if not finished
     */
    int8_t motionAroundPointAngle(float target_x, float target_y, float target_arc_theta, float robot_target_th, uint16_t radius_point, uint16_t minmax_vel_linear, uint16_t minmax_vel_angular, uint8_t flag = 0);
    /**
     * @brief Motion transformation to world
     * @param vel_x Linear velocity in x-axis
     * @param vel_y Linear velocity in y-axis
     * @param vel_theta Angular velocity
     */
    void worldToLocalMotion(float vel_x, float vel_y, float vel_theta, bool is_ruckig = 0);

    int8_t motionToPointRecvBall(float target_x, float target_y, float target_theta, uint16_t minmax_vel_linear, uint16_t minmax_vel_angular, uint8_t flag = 0, float ball_vx = 0.0f, float ball_vy = 0.0f);
    /**
     * @brief Angle to target degree getter
     * @param target_x Target x position
     * @param target_y Target y position
     * @return Angle target (degree)
     */
    float angleToTarget(float target_x, float target_y);
    /**
     * @brief Angle to target degree getter
     * @param target_x Target x position
     * @param target_y Target y position
     * @param base_x Base x position
     * @param base_y Base y position
     * @return Angle target (degree)
     */
    float angleToTarget(float target_x, float target_y, float base_x, float base_y);

    uint8_t receiveBall(float vel_linear = 250, float vel_angular = 200);
    uint8_t receiveBall2(float default_x, float default_y, float vel_linear = 250, float vel_angular = 200);

    bool passBall(float target_x, float target_y, float kick_power, float tolerance);

    uint8_t catchingBall(float vel_linear = 250, float vel_angular = 200, uint8_t flag = 0);
    uint8_t receiveBall3(float default_x, float default_y, float vel_linear = 250, float vel_angular = 200);

    uint8_t motionAngleControl(float vx_local, float vy_local, float target_theta, float vtheta);
    uint8_t motionAngleControlPivot(float vx_local, float vy_local, float target_theta, float vtheta);
    avoid_obs_t ObstacleAvoidance(float vx_input, float vy_input, float angle_tolerance = 25, float dist = 100, uint8_t flags = nearest_pixel);

    uint8_t hadapGawangVision(float vx_local, float vy_local, float vtheta);

    int8_t pivotMotion(float target_th, float vth, float kp_angular = 2.5);
    int8_t ruckigToPoint(float target_x, float target_y, float target_theta, uint16_t minmax_vel_linear, uint16_t minmax_vel_angular, uint8_t flags = 0);
    int8_t ruckigToPoint3(float target_x, float target_y, float target_theta, uint16_t minmax_vel_linear, uint16_t minmax_vel_angular, uint8_t flags = 0);

    obstacle_checking_t obstacleChecking(float theta, float tolerance = 25, float dist = 100, uint8_t flags = NORMAL_OBSTACLE);

    int8_t motionAroundBall(float target_arc_theta, float robot_target_th, uint16_t radius_point, uint16_t minmax_vel_linear, uint16_t minmax_vel_angular, uint8_t flag = 0);

    float RobotAngletoPoint(float x, float y)
    {
        return atan2(y - final_pose->y, x - final_pose->x) RAD2DEG;
    }

    float RobotDist2Point(float _x, float _y)
    {
        return pythagoras(final_pose->x, final_pose->y, _x, _y);
    }

    float RobotAngletoBall()
    {
        if (ball->status > 0)
            return RobotAngletoPoint(ball->pos.x, ball->pos.y);

        return final_pose->theta;
    }

    float BallAngletoEnemyGoal()
    {
        return atan2(FIELD_Y_1 - ball->pos.y, FIELD_X_1_2 - ball->pos.x) RAD2DEG;
    }

    float BallAngletoOurGoal()
    {
        return atan2(FIELD_Y_0 - ball->pos.y, FIELD_X_1_2 - ball->pos.x) RAD2DEG;
    }

    int AutomaticKickerPos(uint8_t power)
    {
        float target_points[2] = { 400, 1200 };
        float point_distance = pythagoras(final_pose->x, final_pose->y, target_points[0], target_points[1]);

        int buffer_kicker_pos = 800;

        return (0.0013 * point_distance * point_distance - 1.1179 * point_distance + 825);
    }

    target_goal_t sudut_robot_ke_gawang_lawan();
    void loadConfig();
};

#endif