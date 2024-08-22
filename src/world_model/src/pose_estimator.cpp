#include "world_model/pose_estimator.hpp"

// #define CALIBRATE_GAIN_OFFSET

int main(int argc, char** argv)
{
    ros::init(argc, argv, "pose_estimator");
    ros::NodeHandle nh;
    ros::MultiThreadedSpinner spinner(4);

    pub_pose = nh.advertise<geometry_msgs::Pose2D>("/world_model/robot/pose", 1);
    pub_vel = nh.advertise<geometry_msgs::Pose2D>("/world_model/robot/vel", 1);
    pub_icp_target_offset = nh.advertise<geometry_msgs::Pose2D>("/world_model/robot/icp_target_offset", 1);
    sub_stm2pc = nh.subscribe<iris_msgs::stm2pc>("/hardware/stm2pc", 1, callbackSubcribeSTM2PC);
    sub_d_icp = nh.subscribe("/world_model/line/d_icp", 1, callbackSubscribeDICP);
    sub_pose_offset = nh.subscribe("/world_model/robot/pose_offset", 1, callbackSubscribePoseOffset);
    tim_routine = nh.createTimer(ros::Duration(0.01), callbackRoutine);

    loadConfig();
    setOffset(init_offset);

    spinner.spin();
    return 0;
}

void callbackRoutine(const ros::TimerEvent& event)
{
    // TODO :: JANCOK
    robot_vel_local.x = -(left_enc.speed * cosf(135 DEG2RAD) + right_enc.speed * sinf(225 DEG2RAD)) ENC2CM;
    robot_vel_local.y = -(left_enc.speed * sinf(135 DEG2RAD) + right_enc.speed * cosf(225 DEG2RAD)) ENC2CM;
    robot_vel_local.theta = gyro.speed;

    // printf("local : %f %f %f\n", (robot_vel_local.x), (robot_vel_local.y), robot_vel_local.theta);
    // printf("final theta : %.f\n", final_pose.theta);

    robot_vel.x = robot_vel_local.x * sinf(final_pose.theta DEG2RAD) + robot_vel_local.y * cosf(final_pose.theta DEG2RAD);
    robot_vel.y = robot_vel_local.x * -cosf(final_pose.theta DEG2RAD) + robot_vel_local.y * sinf(final_pose.theta DEG2RAD);
    robot_vel.theta = robot_vel_local.theta;

    robot_vel.theta > 180 ? robot_vel.theta -= 360 : robot_vel.theta < -180 ? robot_vel.theta += 360
                                                                            : robot_vel.theta;

    final_pose.x += robot_vel.x;
    final_pose.y += robot_vel.y;
    final_pose.theta += robot_vel.theta;

#ifdef CALIBRATE_GAIN_OFFSET
    printf("%f %f %f\n", final_pose.x, final_pose.y, final_pose.theta);
#endif

    final_pose.theta > 180 ? final_pose.theta -= 360 : final_pose.theta < -180 ? final_pose.theta += 360
                                                                               : final_pose.theta;
#ifndef CALIBRATE_GAIN_OFFSET
    pose_msg.theta = final_pose.theta;
    pose_msg.x = final_pose.x + estimator_config.mechanical_offset_gain.x * cosf(final_pose.theta DEG2RAD) + estimator_config.mechanical_offset.x;
    pose_msg.y = final_pose.y + estimator_config.mechanical_offset_gain.y * sinf(final_pose.theta DEG2RAD) + estimator_config.mechanical_offset.y;
#else
    pose_msg.x = final_pose.x;
    pose_msg.y = final_pose.y;
    pose_msg.theta = final_pose.theta;
#endif

    // printf("%.2f %.2f %.2f\n", pose_msg.x, pose_msg.y, pose_msg.theta);

    pub_pose.publish(pose_msg);

    vel_msg.x = robot_vel.x * 100;
    vel_msg.y = robot_vel.y * 100;
    vel_msg.theta = robot_vel.theta * 100;
    pub_vel.publish(vel_msg);
}

void callbackSubscribePoseOffset(const geometry_msgs::Pose2DConstPtr& msg)
{
    setOffset(msg->x, msg->y, msg->theta);
}

void callbackSubcribeSTM2PC(const iris_msgs::stm2pcConstPtr& msg)
{
    static bool init = true;
    static uint8_t odom_calibrated = 0;

    static double last_time_stm_update = 0;

    double time_now = ros::Time::now().toSec();

    if (time_now - last_time_stm_update > 1) {
        ROS_WARN("[Pose Estimator] INIT STM2PC");
        init = true;
    }

    if (msg->odom_buffer_th == 0.0) {
        ROS_ERROR("[Pose Estimator] ODOM NOT CALIBRATED");
    }

    last_time_stm_update = time_now;

    left_enc.prev_px = left_enc.curr_px;
    left_enc.curr_px = msg->left_enc_px;
    left_enc.speed = left_enc.curr_px - left_enc.prev_px;

    right_enc.prev_px = right_enc.curr_px;
    right_enc.curr_px = msg->right_enc_px;
    right_enc.speed = right_enc.curr_px - right_enc.prev_px;

    gyro.prev_px = gyro.curr_px;
    gyro.curr_px = msg->odom_buffer_th;
    gyro.speed = gyro.prev_px - gyro.curr_px;
    // ROS_WARN("GYRO: %.2f %.2f || %.2f", gyro.prev_px, gyro.curr_px, gyro.speed);

    if (init) {
        // setOffset(init_offset);
        left_enc.prev_px = left_enc.curr_px;
        left_enc.speed = 0;
        right_enc.prev_px = right_enc.curr_px;
        right_enc.speed = 0;
        gyro.prev_px = gyro.curr_px;
        gyro.speed = 0;
        init = false;
    }
}

void loadConfig()
{
    char* robot_num;
    robot_num = getenv("ROBOT");
    char config_file[100], config_file2[100];
    std::string current_dir = ros::package::getPath("world_model");
    sprintf(config_file, "%s/../../config/IRIS%s/static_conf.yaml", current_dir.c_str(), robot_num);

    YAML::Node config = YAML::LoadFile(config_file);
    init_offset.x = config["Init"]["pose_x"].as<float>();
    init_offset.y = config["Init"]["pose_y"].as<float>();
    init_offset.theta = config["Init"]["pose_th"].as<float>();
    estimator_config.icp_gain = config["Pose_Estimator"]["icp_gain"].as<float>();
    estimator_config.mechanical_offset.x = config["Pose_Estimator"]["mechanical_offset_x"].as<float>();
    estimator_config.mechanical_offset.y = config["Pose_Estimator"]["mechanical_offset_y"].as<float>();
    estimator_config.mechanical_offset_gain.x = config["Pose_Estimator"]["mechanical_offset_gain_x"].as<float>();
    estimator_config.mechanical_offset_gain.y = config["Pose_Estimator"]["mechanical_offset_gain_y"].as<float>();
}

void setOffset(float x, float y, float theta)
{
    final_pose.theta = theta;
    final_pose.x = x - (estimator_config.mechanical_offset_gain.x * cosf(final_pose.theta DEG2RAD) + estimator_config.mechanical_offset.x);
    final_pose.y = y - (estimator_config.mechanical_offset_gain.y * sinf(final_pose.theta DEG2RAD) + estimator_config.mechanical_offset.y);
}
void setOffset(pose2d_t offset_pose)
{
    final_pose.theta = offset_pose.theta;
    final_pose.x = offset_pose.x - (estimator_config.mechanical_offset_gain.x * cosf(final_pose.theta DEG2RAD) + estimator_config.mechanical_offset.x);
    final_pose.y = offset_pose.y - (estimator_config.mechanical_offset_gain.y * sinf(final_pose.theta DEG2RAD) + estimator_config.mechanical_offset.y);
}

void callbackSubscribeDICP(const geometry_msgs::Pose2DConstPtr& msg)
{
    float dx_icp = msg->x;
    float dy_icp = msg->y;
    float dth_icp = msg->theta;

    pose2d_t center_robot_pose;
    center_robot_pose.theta = final_pose.theta;
    center_robot_pose.x = final_pose.x + estimator_config.mechanical_offset_gain.x * cosf(final_pose.theta DEG2RAD) + estimator_config.mechanical_offset.x;
    center_robot_pose.y = final_pose.y + estimator_config.mechanical_offset_gain.y * sinf(final_pose.theta DEG2RAD) + estimator_config.mechanical_offset.y;

    // if (center_robot_pose.x > 800) {
    //     return;
    // }

    // pose_msg.x = final_pose.x + estimator_config.mechanical_offset_gain.x * cosf(final_pose.theta DEG2RAD) + estimator_config.mechanical_offset.x;
    // pose_msg.y = final_pose.y + estimator_config.mechanical_offset_gain.y * sinf(final_pose.theta DEG2RAD) + estimator_config.mechanical_offset.y;

    // geometry_msgs::Pose2D icp_target_offset;
    // icp_target_offset.x = center_robot_pose.x + dx_icp;
    // icp_target_offset.y = center_robot_pose.y + dy_icp;
    // icp_target_offset.theta = center_robot_pose.theta + dth_icp;
    // pub_icp_target_offset.publish(icp_target_offset);

    center_robot_pose.x = (1 - estimator_config.icp_gain) * center_robot_pose.x + estimator_config.icp_gain * (center_robot_pose.x + dx_icp);
    center_robot_pose.y = (1 - estimator_config.icp_gain) * center_robot_pose.y + estimator_config.icp_gain * (center_robot_pose.y + dy_icp);
    center_robot_pose.theta = (1 - estimator_config.icp_gain) * center_robot_pose.theta + estimator_config.icp_gain * (center_robot_pose.theta + dth_icp);
    if (center_robot_pose.theta > 180)
        center_robot_pose.theta -= 360;
    else if (center_robot_pose.theta < -180)
        center_robot_pose.theta += 360;

    // setOffset(center_robot_pose.x, center_robot_pose.y, center_robot_pose.theta);

    geometry_msgs::Pose2D icp_target_offset;
    icp_target_offset.x = center_robot_pose.x + dx_icp;
    icp_target_offset.y = center_robot_pose.y + dy_icp;
    icp_target_offset.theta = center_robot_pose.theta + dth_icp;
    pub_icp_target_offset.publish(icp_target_offset);

#ifndef CALIBRATE_GAIN_OFFSET

    // ROS_WARN("ICP OFFSET: %.2f %.2f %.2f || d: %.2f %.2f %.2f || %.2f %.2f %.2f",
    //     center_robot_pose.x, center_robot_pose.y, center_robot_pose.theta, dx_icp, dy_icp, dth_icp,
    //     center_robot_pose.x + dx_icp, center_robot_pose.y + dy_icp, center_robot_pose.theta + dth_icp);

    final_pose.theta = center_robot_pose.theta;
    final_pose.x = center_robot_pose.x - (estimator_config.mechanical_offset_gain.x * cosf(final_pose.theta DEG2RAD) + estimator_config.mechanical_offset.x);
    final_pose.y = center_robot_pose.y - (estimator_config.mechanical_offset_gain.y * sinf(final_pose.theta DEG2RAD) + estimator_config.mechanical_offset.y);
#endif
}
