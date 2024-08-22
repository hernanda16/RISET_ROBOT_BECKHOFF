#include "hardware/stm_ballhandling.hpp"

int main(int argc, char** argv)
{
    ros::init(argc, argv, "stm_ballhandling");
    ros::NodeHandle nh;
    ros::MultiThreadedSpinner spinner(0);

    loadConfig();

    pub_to_stm = nh.advertise<std_msgs::Int16MultiArray>("/hardware/ballhandling/stm", 1);
    pub_to_motor = nh.advertise<std_msgs::Float32MultiArray>("/hardware/ballhandling/motor", 1);
    pub_state_ballhandling = nh.advertise<std_msgs::UInt8>("/hardware/ballhandling/status", 1);
    pub_hysteresis_ball = nh.advertise<std_msgs::UInt8>("/hardware/ballhandling/hysteresis", 1);
    pub_pwm_dribble_aux = nh.advertise<std_msgs::Int16MultiArray>("/hardware/ballhandling/dribble_aux_pwm", 1);

    sub_stm2pc = nh.subscribe("/hardware/stm2pc", 1, callbackSubcribeSTM2PC);
    sub_pc2stm = nh.subscribe("/master/pc2stm", 1, callbackSubcribePC2STM);
    sub_v_robot_actuation = nh.subscribe("/master/robot/vel", 1, callbackSubscribersub_v_robot_actuation);

    // sub_final_pose = nh.subscribe("/world_model/robot/pose", 1, callbackSubscribeFinalPose);

    tim_routine = nh.createTimer(ros::Duration(0.01), callbackRoutine);

    spinner.spin();
    return 0;
}

void callbackSubcribeSTM2PC(const iris_msgs::stm2pcConstPtr& msg)
{
    stm_data.button1_pressed = !((msg->buttons & 0b00000001) >> 0);
    stm_data.button2_pressed = !((msg->buttons & 0b00000010) >> 0x01);
    stm_data.button3_pressed = !((msg->buttons & 0b00000100) >> 0x02);
    stm_data.button4_pressed = !((msg->buttons & 0b00001000) >> 0x03);
    stm_data.button5_pressed = !((msg->buttons & 0b00010000) >> 0x04);
    stm_data.toggle1_pressed = !((msg->buttons & 0b00100000) >> 0x05);
    stm_data.toggle2_pressed = !((msg->buttons & 0b01000000) >> 0x06);
    stm_data.toggle3_pressed = !((msg->buttons & 0b10000000) >> 0x07);
    stm_data.battery = msg->battery;

    // if (stm_data.button2_pressed) {
    //     conf.perfect_backward_arm[0] = 60;
    //     conf.perfect_backward_arm[1] = 60;
    // } else if (stm_data.button3_pressed) {
    //     conf.perfect_backward_arm[0] = 85;
    //     conf.perfect_backward_arm[1] = 85;
    // }

    /**
     * @brief Connection to Basestation
     */
    static uint8_t prev_toogle_2 = !((msg->buttons & 0b01000000) >> 0x06);

    /**
     * @brief GP sensors is used to detect ball that close to robot
     */
    curr_gp_value[0] = msg->gp_sensors_left;
    curr_gp_value[1] = msg->gp_sensors_right;

    /**
     * @brief System enable logic control
     * @note System will be enabled if toogle 3 is pressed
     */
    CLEAR_BIT(system_flags, SYSTEM_ENABLE);

    /**
     * @brief Control dribble arm (as a feedback)
     */
    curr_arm_throttle[0] = msg->left_throttle;
    curr_arm_throttle[1] = msg->right_throttle;

    /**
     * @brief Kicker state machine
     */
    curr_kicker_position = msg->current_kicker_position;

    /**
     * @brief Feedback of dribble speed (cnts/100ms)
     */
    dribble_vx[0] = msg->left_dribble_enc_vx;
    dribble_vx[1] = msg->right_dribble_enc_vx;

    /**
     * @brief STM epoch to determine STM state (alive or not)
     */
    last_time_stm_update = ros::Time::now().toSec();

    /**
     * @brief Online calibration is enabled if toogle 2 is in pressed state
     */
#ifdef ONLINE_CALIBRATION
    if (!((msg->buttons & 0b01000000) >> 0x06) != prev_toogle_2)
        online_calibration = 1;
    prev_toogle_2 = !((msg->buttons & 0b01000000) >> 0x06);
#endif
}

void callbackSubcribePC2STM(const iris_msgs::pc2stmConstPtr& msg)
{
    /**
     * @brief Control State Machine of kicking mechanism
     */
    kick_mode = msg->kicker_mode;
    kick_power = msg->kicker_power;
    kick_pos = msg->kicker_position;
    global_kicker_position = msg->global_kicker_position;
    dribble_up = msg->dribble_up;
    enable_auto_correct_position = msg->enable_auto_correct_position;
    force_dribble_control = msg->force_dribble_control;
    long_dribble_control = msg->long_dribble_control;
    stand_by = msg->stand_by;

    // printf("STM: %d\n", force_dribble_control);
}

void callbackSubcribeOdometry(const geometry_msgs::Pose2DConstPtr& msg)
{
    robot_vel_master[0] = msg->x;
    robot_vel_master[1] = msg->y;
    robot_vel_master[2] = msg->theta;
}

void callbackSubcribeOnlineCalibration(const std_msgs::UInt8ConstPtr& msg)
{
    if (msg->data == 1)
        online_calibration = msg->data;
}

void callbackSubscribeFinalPose(const geometry_msgs::Pose2DConstPtr& msg)
{
    // float y = msg->x * -cosf((msg->theta - 90) * DEG2RAD) + msg->y * sinf((msg->theta - 90) * DEG2RAD);
    // float x = msg->x * sinf((msg->theta - 90) * DEG2RAD) + msg->y * cosf((msg->theta - 90) * DEG2RAD);
    // float th = msg->theta;

    // static float prev_x = x;
    // static float prev_y = y;
    // static float prev_th = th;

    // v_robot[0] = x - prev_x;
    // v_robot[1] = y - prev_y;
    // v_robot[2] = th - prev_th;

    // prev_x = x;
    // prev_y = y;
    // prev_th = th;
}

uint8_t onlineCalibration()
{
    static uint8_t calibration_state = 0;
    static double last_call = ros::Time::now().toSec();
    static double early_high_state = 0;

    /**
     * @brief Reset if this function is not call within 2 secs
     */
    if (ros::Time::now().toSec() - last_call > 2 || calibration_state == 4) {
        calibration_state = 0;
    }

    last_call = ros::Time::now().toSec();

    ROS_WARN("ONLINE CALIBRATION %d %d %d", calibration_state, curr_arm_throttle[0], curr_arm_throttle[1]);

    switch (calibration_state) {
    case 0:
        /* Record low arm throttle */
        conf.low_dribble_arm_calibration[0] = curr_arm_throttle[0];
        conf.low_dribble_arm_calibration[1] = curr_arm_throttle[1];
        calibration_state = 1;

        break;
    case 1:
        if (curr_gp_value[0] > conf.status_ball_threshold[0] && curr_gp_value[1] > conf.status_ball_threshold[1]) {
            calibration_state = 2;
            early_high_state = ros::Time::now().toSec();
        }
        break;
    case 2:
        /* Wait for ball to ensure that ball has been caught, so the dribble was in high state */
        if (ros::Time::now().toSec() - early_high_state > 3)
            calibration_state = 3;
        break;
    case 3:
        /* Record high arm throttle */
        conf.high_dribble_arm_calibration[0] = curr_arm_throttle[0];
        conf.high_dribble_arm_calibration[1] = curr_arm_throttle[1];

        has_calibrated = 1;
        calibration_state = 4;
        saveConfig();
        break;
    }

    return (calibration_state != 4);
}

void loadConfig()
{
    char* robot_num = getenv("ROBOT");
    char static_conf[100], dynamic_conf[100];
    std::string current_dir = ros::package::getPath("hardware");
    sprintf(static_conf, "%s/../../config/IRIS%s/static_conf.yaml", current_dir.c_str(), robot_num);
    sprintf(dynamic_conf, "%s/../../config/IRIS%s/dynamic_conf.yaml", current_dir.c_str(), robot_num);

    YAML::Node config1 = YAML::LoadFile(static_conf);
    conf.gp_sensor_threshold[0] = config1["Ballhandling"]["dribble_gp_left"].as<int>();
    conf.gp_sensor_threshold[1] = config1["Ballhandling"]["dribble_gp_right"].as<int>();
    conf.status_ball_threshold[0] = config1["Ballhandling"]["ball_status_gp_left"].as<int>();
    conf.status_ball_threshold[1] = config1["Ballhandling"]["ball_status_gp_right"].as<int>();
    conf.perfect_backward_arm[0] = config1["Ballhandling"]["perfect_backward_arm_left"].as<int>();
    conf.perfect_backward_arm[1] = config1["Ballhandling"]["perfect_backward_arm_right"].as<int>();
    conf.perfect_forward_arm[0] = config1["Ballhandling"]["perfect_forward_arm_left"].as<int>();
    conf.perfect_forward_arm[1] = config1["Ballhandling"]["perfect_forward_arm_right"].as<int>();
    conf.dribble_arm_kick[0] = config1["Ballhandling"]["dribble_arm_left_kick"].as<int>();
    conf.dribble_arm_kick[1] = config1["Ballhandling"]["dribble_arm_right_kick"].as<int>();
    conf.kPID_throttle_control[0] = config1["Ballhandling"]["kp_throttle_control"].as<float>();
    conf.kPID_throttle_control[1] = config1["Ballhandling"]["ki_throttle_control"].as<float>();
    conf.kPID_throttle_control[2] = config1["Ballhandling"]["kd_throttle_control"].as<float>();
    conf.kPID_dribble_vel[0] = config1["Ballhandling"]["kp_dribble_vel"].as<float>();
    conf.kPID_dribble_vel[1] = config1["Ballhandling"]["ki_dribble_vel"].as<float>();
    conf.kPID_dribble_vel[2] = config1["Ballhandling"]["kd_dribble_vel"].as<float>();
    conf.perfect_dribble_up[0] = config1["Ballhandling"]["perfect_backward_arm_left"].as<int>();
    conf.perfect_dribble_up[1] = config1["Ballhandling"]["perfect_backward_arm_right"].as<int>();

    YAML::Node config2 = YAML::LoadFile(dynamic_conf);
    conf.low_dribble_arm_calibration[0] = config1["Ballhandling"]["low_dribble_arm_left"].as<int>();
    conf.low_dribble_arm_calibration[1] = config1["Ballhandling"]["low_dribble_arm_right"].as<int>();
    conf.high_dribble_arm_calibration[0] = config1["Ballhandling"]["high_dribble_arm_left"].as<int>();
    conf.high_dribble_arm_calibration[1] = config1["Ballhandling"]["high_dribble_arm_right"].as<int>();

    printf("DRIBBLE CONFIG %d %d %d %d %d %d %d %d %d %d\n",
        conf.gp_sensor_threshold[0],
        conf.gp_sensor_threshold[1],
        conf.low_dribble_arm_calibration[0],
        conf.low_dribble_arm_calibration[1],
        conf.high_dribble_arm_calibration[0],
        conf.high_dribble_arm_calibration[1],
        conf.perfect_backward_arm[0],
        conf.perfect_backward_arm[1],
        conf.dribble_arm_kick[0],
        conf.dribble_arm_kick[1]);
    printf("PID throttle %.2f %.2f %.2f\n",
        conf.kPID_throttle_control[0],
        conf.kPID_throttle_control[1],
        conf.kPID_throttle_control[2]);
    printf("PID velocity %.2f %.2f %.2f\n",
        conf.kPID_dribble_vel[0],
        conf.kPID_dribble_vel[1],
        conf.kPID_dribble_vel[2]);
}

void saveConfig()
{
    char config_file[100];
    std::string current_dir = ros::package::getPath("vision");
    sprintf(config_file, "%s/../../config/IRIS%s/dynamic_conf.yaml", current_dir.c_str(), getenv("ROBOT"));

    printf("saved %d %d %d %d\n", conf.low_dribble_arm_calibration[0], conf.low_dribble_arm_calibration[1],
        conf.high_dribble_arm_calibration[0], conf.high_dribble_arm_calibration[1]);

    YAML::Node config = YAML::LoadFile(config_file);
    config["Ballhandling"]["low_dribble_arm_left"] = std::to_string(conf.low_dribble_arm_calibration[0]);
    config["Ballhandling"]["low_dribble_arm_right"] = std::to_string(conf.low_dribble_arm_calibration[1]);
    config["Ballhandling"]["high_dribble_arm_left"] = std::to_string(conf.high_dribble_arm_calibration[0]);
    config["Ballhandling"]["high_dribble_arm_right"] = std::to_string(conf.high_dribble_arm_calibration[1]);

    std::ofstream fout;
    fout.open(config_file, std::ios::out);
    fout << config;
    fout.close();
}

void kick(float* throttle_norm, float* return_power, int16_t* return_pos, int16_t* return_mode, float* return_dribble)
{
    static const double timeout_control = 0.4; // second
    static double time_start_control = 0;

    /**s
     * @brief State machine for kicking mechanism
     * @note 0: Reset State
     * @note 1: Kicker Position Aim
     * @note 2: Throttle Aim
     * @note 3: Kicker Position and Throttle Aim
     * @note 56: Only Charge
     * @note 57: Only Discharge
     * @note 99: Kick
     */
    static uint8_t kick_state = 0;
    static double last_call = 0;
    static uint8_t throttle_sp_for_kicking[2];
    static float return_buffer[6];
    static uint8_t perfect_cntr = 1;

    /* Reset state if this function not called within 0.8 secs.. */
    if (ros::Time::now().toSec() - last_call > 0.8)
        kick_state = 0;

    // ROS_WARN("KICK STATE %d", kick_state); //TODO: UNCOMMENT THIS

    switch (kick_state) {
    case 0:
        printf("KICKTATES 000000000\n");
        if (kick_mode == NO_AIM)
            kick_state = 99;
        else if (kick_mode == KICKER_POSITION_AIM)
            kick_state = 1;
        else if (kick_mode == THROTTLE_AIM)
            kick_state = 2;
        else if (kick_mode == (KICKER_POSITION_AIM | THROTTLE_AIM))
            kick_state = 3;
        else if (kick_mode == ONLY_CHARGE) {
            *return_mode = 56;
            *return_power = 0;
        } else if (kick_mode == ONLY_DISCHARGE) {
            *return_mode = 57;
            *return_power = 0;
        }

        /* Handle kicker position to current kicker position, so it will maintain to it's position */
        *return_pos = curr_kicker_position;

        return_dribble[0] = -200;
        return_dribble[1] = 200;

        perfect_cntr = 0;

        time_start_control = ros::Time::now().toSec();
        break;
        /* Aiming the kicker position based on height */
    case 1:
        return_dribble[0] = -50;
        return_dribble[1] = 50;
        *return_pos = kick_pos;
        if (DABS(kick_pos - curr_kicker_position) < 10)
            kick_state = 4;
        break;
    case 4:
        return_dribble[0] = -50;
        return_dribble[1] = 50;
        kick_state = 99;
        break;

        /* Aiming the ball with throttle control */
    case 2:
        throttle_sp_for_kicking[0] = conf.dribble_arm_kick[0];
        throttle_sp_for_kicking[1] = conf.dribble_arm_kick[1];
        // integral_arm[0] = 0;
        // integral_arm[1] = 0;
        throttleControl(throttle_norm, throttle_sp_for_kicking, return_buffer, conf.kPID_throttle_control);

        return_buffer[0] = return_buffer[2];
        return_buffer[1] = return_buffer[3];

        dribbleVelocityControl(dribble_vx, return_buffer, return_dribble, conf.kPID_dribble_vel);

        // printf("PERFECT CTR %d|| throttle_norm 1 %.2f || throttle_norm 2 %.2f \n", perfect_cntr, fabs(throttle_norm[0] - throttle_sp_for_kicking[0]), fabs(throttle_norm[1] - throttle_sp_for_kicking[1]));

        if (fabs(throttle_norm[0] - throttle_sp_for_kicking[0]) < 15 && fabs(throttle_norm[1] - throttle_sp_for_kicking[1]) < 15) {
            perfect_cntr++;
        }
        if (perfect_cntr > 5)
            kick_state = 5;

        if (ros::Time::now().toSec() - time_start_control > timeout_control)
            kick_state = 5;
        break;
    case 5:
        return_dribble[0] = -100;
        return_dribble[1] = 100;
        kick_state = 99;
        break;

        /* Aiming the kicker position and control the ball with throttle */
    case 3:
        *return_pos = kick_pos;

        throttle_sp_for_kicking[0] = conf.dribble_arm_kick[0];
        throttle_sp_for_kicking[1] = conf.dribble_arm_kick[1];
        // integral_arm[0] = 0;
        // integral_arm[1] = 0;
        throttleControl(throttle_norm, throttle_sp_for_kicking, return_buffer, conf.kPID_throttle_control);

        return_buffer[0] = return_buffer[2];
        return_buffer[1] = return_buffer[3];

        dribbleVelocityControl(dribble_vx, return_buffer, return_dribble, conf.kPID_dribble_vel);

        // printf("PERFECT CTR %d|| throttle_norm 1 %.2f || throttle_norm 2 %.2f \n", perfect_cntr, fabs(throttle_norm[0] - throttle_sp_for_kicking[0]), fabs(throttle_norm[1] - throttle_sp_for_kicking[1]));

        if (fabs(throttle_norm[0] - throttle_sp_for_kicking[0]) < 15 && fabs(throttle_norm[1] - throttle_sp_for_kicking[1]) < 15) {
            perfect_cntr++;
        }
        if (perfect_cntr > 5 && DABS(kick_pos - curr_kicker_position) < 10)
            kick_state = 6;

        if (ros::Time::now().toSec() - time_start_control > timeout_control)
            kick_state = 6;
        break;

    case 6:
        return_dribble[0] = -100;
        return_dribble[1] = 100;
        kick_state = 99;
        break;

        /* Kick */
    case 99:
        return_dribble[0] = -50;
        return_dribble[1] = 50;
        *return_mode = 58;
        *return_power = kick_power;
        break;
    }

    last_call = ros::Time::now().toSec();
}

void throttleControl(float* data, uint8_t* sp, float* ret, float* PID_conf)
{
    static float prev_error[2];
    static float proportional[2];
    static float derivative[2];
    float error[2];

    error[0] = data[0] - sp[0];
    error[1] = sp[1] - data[1];

    // TODO : MAS PANDU

    // if (error[0] < 0)
    // {
    //     // printf("KIRI : %f < 0\n", error[0]);
    //     error[0] *= 1.5;
    // }
    // if (error[1] < 0)
    // {
    //     // printf("KANAN : %f < 0\n", error[1]);
    //     error[1] *= 1.5;
    // }
    proportional[0] = PID_conf[0] * error[0];
    proportional[1] = PID_conf[0] * error[1];

    integral_arm[0] += PID_conf[1] * error[0];
    integral_arm[1] += PID_conf[1] * error[1];

    derivative[0] = PID_conf[2] * (error[0] - prev_error[0]);
    derivative[1] = PID_conf[2] * (error[1] - prev_error[1]);

    if (integral_arm[0] > 3000)
        integral_arm[0] = 3000;
    else if (integral_arm[0] < -3000)
        integral_arm[0] = -3000;

    if (integral_arm[1] > 3000)
        integral_arm[1] = 3000;
    else if (integral_arm[1] < -3000)
        integral_arm[1] = -3000;

    ret[2] = proportional[0] + integral_arm[0] + derivative[0];
    ret[3] = proportional[1] + integral_arm[1] + derivative[1];

    if (ret[2] > 7500)
        ret[2] = 7500;
    else if (ret[2] < -7500)
        ret[2] = -7500;

    if (ret[3] > 7500)
        ret[3] = 7500;
    else if (ret[3] < -7500)
        ret[3] = -7500;

    prev_error[0] = error[0];
    prev_error[1] = error[1];
}

void throttleControlFloat(float* data, float* sp, float* ret, float* PID_conf)
{
    static float prev_error[2];
    static float proportional[2];
    // static floatintegral_arm[2];
    static float derivative[2];
    float error[2];

    error[0] = data[0] - sp[0];
    error[1] = sp[1] - data[1];

    // printf("error: %.2f %.2f\n", error[0], error[1]);

    proportional[0] = PID_conf[0] * error[0];
    proportional[1] = PID_conf[0] * error[1];

    integral_arm[0] += PID_conf[1] * error[0];
    integral_arm[1] += PID_conf[1] * error[1];

    derivative[0] = PID_conf[2] * (error[0] - prev_error[0]);
    derivative[1] = PID_conf[2] * (error[1] - prev_error[1]);

    if (integral_arm[0] > 7500)
        integral_arm[0] = 7500;
    else if (integral_arm[0] < -7500)
        integral_arm[0] = -7500;

    if (integral_arm[1] > 7500)
        integral_arm[1] = 7500;
    else if (integral_arm[1] < -7500)
        integral_arm[1] = -7500;

    // printf("KK: %.2f %.2f %.2f\n", PID_conf[0], PID_conf[1], PID_conf[2]);
    // printf("pid0: %.2f %.2f %.2f \n", proportional[0],integral_arm[0], derivative[0]);
    // printf("pid1: %.2f %.2f %.2f \n", proportional[1],integral_arm[1], derivative[1]);

    // printf("=====================================\n");

    ret[2] = proportional[0] + integral_arm[0] + derivative[0];
    ret[3] = proportional[1] + integral_arm[1] + derivative[1];

    if (ret[2] > 7500)
        ret[2] = 7500;
    else if (ret[2] < -7500)
        ret[2] = -7500;

    if (ret[3] > 7500)
        ret[3] = 7500;
    else if (ret[3] < -7500)
        ret[3] = -7500;

    prev_error[0] = error[0];
    prev_error[1] = error[1];
}

/**
 * It same as in main timer callback,
 * but this encapsulated on a function
 */
void dribbleVelocityControl(int16_t* data, float* sp, float* ret, float* PID_conf)
{
    float left_sp = sp[0] * DEG2CNTS / 100;
    float right_sp = sp[1] * DEG2CNTS / 100;

    /* PI Control system for controlling dribble velocity */
    static float pid_error[2];
    static float pid_prev_error[2];

    static float pid_proportional[2];
    static float pid_integral_arm[2];
    static float pid_derivative[2];

    static float pid_output[2];

    pid_error[0] = left_sp - data[0];
    pid_error[1] = right_sp - data[1];

    for (int i = 0; i < 2; i++) {
        pid_proportional[i] = PID_conf[0] * pid_error[i];
        pid_integral_arm[i] += PID_conf[1] * pid_error[i];
        pid_derivative[i] = PID_conf[2] * (pid_error[i] - pid_prev_error[i]);

        if (pid_integral_arm[i] > MAXPWM)
            pid_integral_arm[i] = MAXPWM;
        else if (pid_integral_arm[i] < -MAXPWM)
            pid_integral_arm[i] = -MAXPWM;

        pid_output[i] = pid_proportional[i] + pid_integral_arm[i] + pid_derivative[i];

        if (pid_output[i] > MAXPWM)
            pid_output[i] = MAXPWM;
        else if (pid_output[i] < -MAXPWM)
            pid_output[i] = -MAXPWM;

        pid_prev_error[i] = pid_error[i];

        ret[i] = pid_output[i];
    }
}

void encs2Local2BallVel(float* ret)
{
    static float pose_buffer[3] = { 0, 0, 0 };
    static float prev_robot_pose[3];
    static float final_robot_velocity[3];

    /* Integral velocity to get robot pose */
    pose_buffer[0] += robot_vel_master[0];
    pose_buffer[1] += robot_vel_master[1];

    /* Using matrix translation to center of robot */
    robot_pose[0] = pose_buffer[0] - 8 * cosf(robot_pose[2]);
    robot_pose[1] = pose_buffer[1] - 15 * sinf(robot_pose[2]);

    /* Derivate Pose to get final robot velocity */
    final_robot_velocity[0] = (robot_pose[0] - prev_robot_pose[0]);
    final_robot_velocity[1] = (robot_pose[1] - prev_robot_pose[1]);
    final_robot_velocity[2] = robot_vel_master[2] * 100;

    /* It convert angular velocity to linear velocity */
    float angular_to_linear_velocity;
    angular_to_linear_velocity = final_robot_velocity[2] * BALLROBOTDIST * DEG2RAD;

    /* Set a return value */
    ret[0] = final_robot_velocity[0] - angular_to_linear_velocity;
    ret[1] = final_robot_velocity[1];
    ret[2] = final_robot_velocity[2];

    /* Set prev pose */
    memcpy(prev_robot_pose, robot_pose, 8);
}

void kinematicsV3(float* ball_vel, float* ret)
{
    float ball_velocity_yaw;
    float buffer[2];

    ball_velocity_yaw = ball_vel[0] * BALLROBOTDIST;

    buffer[0] = ball_vel[0] * cosf(60 * DEG2RAD) + ball_vel[1] * sinf(60 * DEG2RAD);
    buffer[1] = ball_vel[0] * cosf(-60 * DEG2RAD) + ball_vel[1] * sinf(-60 * DEG2RAD);

    buffer[0] = buffer[0] / DRIBBLE_RADIUS * RAD2DEG + ball_velocity_yaw;
    buffer[1] = buffer[1] / DRIBBLE_RADIUS * RAD2DEG + ball_velocity_yaw;

    ret[4] = buffer[0];
    ret[5] = buffer[1];
}

void dribbleUpDownControl(float* data, uint8_t* sp, int16_t* ret)
{
    float error[2];
    static float pid_prev_error[2];
    float pid_output[2];
    static float conf_dribble[3] = { 15, 0, 0 };

    // error[0] = data[0] - sp[0];
    error[0] = sp[0] - data[0];
    error[1] = sp[1] - data[1];

    for (int i = 0; i < 2; i++) {
        pid_output[i] = conf_dribble[0] * error[i];

        // printf("Sebelum: %.2f %.2f || %.2f %.2f || %d %d || %.2f %.2f || \n  ", pid_output[0], pid_output[1], error[0], error[1], sp[0], sp[1], data[0], data[1]);
        if (pid_output[i] > MAXPWM)
            pid_output[i] = MAXPWM;
        else if (pid_output[i] < -MAXPWM)
            pid_output[i] = -MAXPWM;

        pid_prev_error[i] = error[i];

        ret[i] = (int16_t)pid_output[i];
    }
}

void throttleControlNoRetract(float* data, uint8_t* sp, float* ret, float* PID_conf)
{
    static float prev_error[2];
    static float proportional[2];
    static float derivative[2];
    float error[2];

    error[0] = data[0] - sp[0];
    error[1] = sp[1] - data[1];

    proportional[0] = PID_conf[0] * error[0];
    proportional[1] = PID_conf[0] * error[1];

    integral_arm[0] += PID_conf[1] * error[0];
    integral_arm[1] += PID_conf[1] * error[1];

    derivative[0] = PID_conf[2] * (error[0] - prev_error[0]);
    derivative[1] = PID_conf[2] * (error[1] - prev_error[1]);

    if (integral_arm[0] > 3000)
        integral_arm[0] = 3000;
    else if (integral_arm[0] < -3000)
        integral_arm[0] = -3000;

    if (integral_arm[1] > 3000)
        integral_arm[1] = 3000;
    else if (integral_arm[1] < -3000)
        integral_arm[1] = -3000;

    ret[2] = proportional[0] + integral_arm[0] + derivative[0];
    ret[3] = proportional[1] + integral_arm[1] + derivative[1];

    if (ret[2] > 0)
        ret[2] = 0;
    if (ret[3] < 0)
        ret[3] = 0;

    if (ret[2] > 7500)
        ret[2] = 7500;
    else if (ret[2] < -7500)
        ret[2] = -7500;

    if (ret[3] > 7500)
        ret[3] = 7500;
    else if (ret[3] < -7500)
        ret[3] = -7500;

    prev_error[0] = error[0];
    prev_error[1] = error[1];
}

void callbackRoutine(const ros::TimerEvent&)
{
    /* Send to STM */
    static int16_t dribble_send[2];
    static float kick_power_send;
    static int16_t kick_pos_send;
    static int16_t kick_mode_send;

    /* Send To Master */
    static uint8_t status_perfect_dribble_send = 0;

    /* Send To Motor for additional vels */
    static float motor_send[2] = { 0, 0 };

    /* Use for calculating */
    static uint8_t status_perfect_dribble = 0;
    static uint8_t status_max_dribble = 0;
    static uint64_t delay_ball_in = 0;
    static uint64_t delay_ball_out = 0;
    static uint64_t delay_perfect_in = 0;
    static float sp_buffer[6] = { 0, 0, 0, 0, 0, 0 };
    static float throttle_norm[2] = { 0, 0 };
    static float ball_vel[3];
    static uint8_t prev_ball_status;

    /* Pointer to throttle stuffs */
    static uint8_t* curr_arm_left = &curr_arm_throttle[0];
    static uint8_t* curr_arm_right = &curr_arm_throttle[1];
    static uint8_t* low_arm_left = &conf.low_dribble_arm_calibration[0];
    static uint8_t* low_arm_right = &conf.low_dribble_arm_calibration[1];
    static uint8_t* high_arm_left = &conf.high_dribble_arm_calibration[0];
    static uint8_t* high_arm_right = &conf.high_dribble_arm_calibration[1];

    static int16_t pwm_dribble_aux[2];

    static float buffer_sp_first[2];
    static uint64_t counter_max_dribble = 0;

    static const float time_to_perfect = 5;
    static double start_time = 0;

    static uint8_t prev_state_hysteresis = 0;
    static float initial_throttle[2];

    static uint64_t counter_hisap = 0;
    static double first_time_caught_ball = 0;

    static uint8_t state_control_dribble = 0;
    static double delta_time_hisap = 0;
    static double waktu_awal_hisap_kedua = 0;

    system_flags &= SYSTEM_ENABLE; // Enable or disable system from STM, see STM2PC callback
    system_flags |= (ros::Time::now().toSec() - last_time_stm_update < DEAD_THRES);

    //===============================================================================================

    /* Dribble online calibration if toggle2 falling or raising edge */
    if (online_calibration) {
        std_msgs::Int16MultiArray msg_pub_vel_dribble;
        msg_pub_vel_dribble.data.push_back(130);
        msg_pub_vel_dribble.data.push_back(-130);
        msg_pub_vel_dribble.data.push_back(0);
        msg_pub_vel_dribble.data.push_back(0);
        msg_pub_vel_dribble.data.push_back(throttle_norm[0]);
        msg_pub_vel_dribble.data.push_back(throttle_norm[1]);

        msg_pub_vel_dribble.data.push_back(0);
        pub_to_stm.publish(msg_pub_vel_dribble);

        online_calibration = onlineCalibration();
        return;
    }

    //===============================================================================================

    /* This will normalize throttle data from 0 to 100 */
    if (*high_arm_left != *low_arm_left && *high_arm_right != *low_arm_right) {
        throttle_norm[0] = ((int)*curr_arm_left - (*low_arm_left)) * 100 / ((*high_arm_left - *low_arm_left));
        throttle_norm[1] = ((int)*curr_arm_right - (*low_arm_right)) * 100 / ((*high_arm_right - *low_arm_right));
    }

    //===============================================================================================

    /* It used to give a hysteresis delay, so if ball was caught, the system won't set ball status caught before the delay was reached */
    if (curr_gp_value[0] > conf.gp_sensor_threshold[0] || curr_gp_value[1] > conf.gp_sensor_threshold[1]) {
        delay_ball_in += 20;
        delay_ball_out = 0;
    } else if (curr_gp_value[0] < conf.gp_sensor_threshold[0] && curr_gp_value[1] < conf.gp_sensor_threshold[1]) {
        delay_ball_in = 0;
        delay_ball_out += 20;
        delay_perfect_in = 0;
        // status_max_dribble = 1;
    }

    /* It used to give a hysteresis delay, so if ball was caught, the system won't set ball status caught before the delay was reached */
    if (delay_ball_in > 100)
        system_flags |= HYSTERESIS;
    else if (delay_ball_out > 200) {
        system_flags &= ~HYSTERESIS;
        counter_max_dribble = 0;
        state_control_dribble = 0;
    }

    //===============================================================================================

    // Ketika salah satu lepas, reset kontrol ke kontrol smooth
    if ((throttle_norm[0] < 1 || throttle_norm[1] < 1) && GET_FLAG(system_flags, HYSTERESIS)) {
        first_time_caught_ball = ros::Time::now().toSec();
        state_control_dribble = 12;
        counter_max_dribble = 0;
        waktu_awal_hisap_kedua = ros::Time::now().toSec();
    }

    // ====================== KICK ===================
    // Don't control dribble velocity with PID
    if (kick_mode == OFFBALLWITHDRIBBLE) {
        std_msgs::Int16MultiArray msg_pub_vel_dribble;
        msg_pub_vel_dribble.data.push_back(-300);
        msg_pub_vel_dribble.data.push_back(300);
        msg_pub_vel_dribble.data.push_back(20);
        msg_pub_vel_dribble.data.push_back(0);
        msg_pub_vel_dribble.data.push_back(throttle_norm[0]);
        msg_pub_vel_dribble.data.push_back(throttle_norm[1]);
        msg_pub_vel_dribble.data.push_back(10);
        pub_to_stm.publish(msg_pub_vel_dribble);

        std_msgs::UInt8 msg_state_ballhandling;
        msg_state_ballhandling.data = state_control_dribble;
        pub_state_ballhandling.publish(msg_state_ballhandling);

        return;
    }
    /* Use kick Finite State Machine here */
    else if (kick_mode != 0) {
        kick(throttle_norm, &kick_power_send, &kick_pos_send, &kick_mode_send, sp_buffer);
        dribble_send[0] = (int16_t)sp_buffer[0];
        dribble_send[1] = (int16_t)sp_buffer[1];

        // ROS_ERROR("%d %d %.2f %d %d", dribble_send[1], dribble_send[0], kick_power_send, kick_pos_send, kick_mode_send);
        std_msgs::Int16MultiArray msg_pub_vel_dribble;
        msg_pub_vel_dribble.data.push_back(dribble_send[1]);
        msg_pub_vel_dribble.data.push_back(dribble_send[0]);
        msg_pub_vel_dribble.data.push_back(kick_power_send * 100);
        msg_pub_vel_dribble.data.push_back(kick_pos_send);
        msg_pub_vel_dribble.data.push_back(throttle_norm[0]);
        msg_pub_vel_dribble.data.push_back(throttle_norm[1]);
        msg_pub_vel_dribble.data.push_back(kick_mode_send);
        pub_to_stm.publish(msg_pub_vel_dribble);

        std_msgs::UInt8 msg_state_ballhandling;
        msg_state_ballhandling.data = state_control_dribble;
        pub_state_ballhandling.publish(msg_state_ballhandling);

        return;
    } else {
        kick_mode_send = 0;
        kick_power_send = 0;
        kick_pos_send = 0;
    }

    if (throttle_norm[0] > 70 && throttle_norm[1] > 70) {
        counter_max_dribble++;
    }

    if (counter_max_dribble > 1) {
        status_max_dribble = 1;
    } else if (counter_max_dribble == 0) {
        status_max_dribble = 0;
    }

    static const double lama_waktu_hisap = 0.2; // detik, 0.5
    static const double lama_waktu_hisap_kedua = 0.1;
    static const double timeout_hisap = 5;
    static uint8_t prev_state_control_dribble = 0;
    static uint64_t counter_perfect = 0;

    // printf("%d %d (%d %d) %.2f (%.2f %.2f) -> %d (%d %d) (%.2f %.2f) (%d) [%d]\n", prev_state_control_dribble, state_control_dribble, GET_FLAG(system_flags, HYSTERESIS), status_perfect_dribble_send, first_time_caught_ball,
    //     initial_throttle[0], initial_throttle[1], counter_max_dribble,
    //     curr_gp_value[0], curr_gp_value[1], throttle_norm[0], throttle_norm[1], counter_perfect,
    //     force_dribble_control);

    //

    // printf("VROBOT: %.2f %.2f %.2f\n", v_robot[0], v_robot[1], v_robot[2]);

    if (prev_state_control_dribble == 12 && state_control_dribble == 2) {
        initial_throttle[0] = throttle_norm[0];
        initial_throttle[1] = throttle_norm[1];
    }

    prev_state_control_dribble = state_control_dribble;

    delta_time_hisap = ros::Time::now().toSec() - first_time_caught_ball;
    switch (state_control_dribble) {
    case 0:

        if (force_dribble_control == 4) {
            sp_buffer[0] = (fabsf(v_robot[1]) * 4);
            sp_buffer[1] = -(fabsf(v_robot[1]) * 4);

            // printf("HALO: %.2f || %.2f\n", sp_buffer[0], v_robot[1]);
        } else {
            if (stand_by) {
                sp_buffer[0] = -4000;
                sp_buffer[1] = 4000;
            } else {
                sp_buffer[0] = -360;
                sp_buffer[1] = 360;
            }
        }
        if (prev_state_hysteresis == 0 && GET_FLAG(system_flags, HYSTERESIS) == 1) {
            first_time_caught_ball = ros::Time::now().toSec();
            dribble_up = 1;

            // if (force_dribble_control == 1) {
            //     initial_throttle[0] = conf.perfect_backward_arm[0];
            //     initial_throttle[1] = conf.perfect_backward_arm[1];
            //     state_control_dribble = 2;
            //     dribble_up = 0;
            // }
            // else {
            // }
            state_control_dribble = 12; //
        }
        status_perfect_dribble_send = 0;
        motor_send[0] = 0;
        motor_send[1] = 0;
        status_max_dribble = 0;
        counter_perfect = 0;

        integral_arm[0] = 0;
        integral_arm[1] = 0;

        status_perfect_dribble_send = 0;

        break;
    case 11: {
        sp_buffer[0] = -720;
        sp_buffer[1] = 720;

        ROS_WARN("Hisap %.2f", delta_time_hisap);

        if (delta_time_hisap > lama_waktu_hisap && status_max_dribble == 1) {
            state_control_dribble = 12;
            waktu_awal_hisap_kedua = ros::Time::now().toSec();
        }
        status_perfect_dribble_send = 0;
        counter_perfect = 0;

        if (enable_auto_correct_position) {
            ROS_WARN("ENABLE AUTO CORRECT POSITION");
            motor_send[0] = throttle_norm[0];
            motor_send[1] = throttle_norm[1];
        } else {
            motor_send[0] = 0;
            motor_send[1] = 0;
        }

        integral_arm[0] = 0;
        integral_arm[1] = 0;

        if (delta_time_hisap > timeout_hisap) {
            printf("RESET CASE 1 BY TIMEOUT %lf\n", delta_time_hisap);
            state_control_dribble = 0;
        }
        status_perfect_dribble_send = 0;

        break;
    }
    case 12: {
        motor_send[0] = 0;
        motor_send[1] = 0;

        // printf("CASE 12: %d\n", force_dribble_control);
        if (force_dribble_control == 2) {
            sp_buffer[0] = -6000; // 1500 vs MR.DEV
            sp_buffer[1] = 6000; // 1500 vs MR.DEV
        } else if (force_dribble_control == 1 || force_dribble_control == 0) {
            sp_buffer[0] = -1200; // 1500 vs MR.DEV
            sp_buffer[1] = 1200; // 1500 vs MR.DEV
        } else if (force_dribble_control == 4) {
            sp_buffer[0] = (fabsf(v_robot[1]) * 4);
            sp_buffer[1] = -(fabsf(v_robot[1]) * 4);
        }

        double delta_time = ros::Time::now().toSec() - waktu_awal_hisap_kedua;
        if (delta_time > lama_waktu_hisap_kedua && status_max_dribble == 1) {
            state_control_dribble = 2;
        }
        status_perfect_dribble_send = 0;
        counter_perfect = 0;

        integral_arm[0] = 0;
        integral_arm[1] = 0;

        // uint8_t used_sp[2];
        // used_sp[0] = 100;
        // used_sp[1] = 100;

        // float custom_pid[3] = {20, 0, 0};
        // throttleControl(throttle_norm, used_sp, sp_buffer, custom_pid);
        // sp_buffer[0] = sp_buffer[2] * 1 + sp_buffer[4] * 0;
        // sp_buffer[1] = sp_buffer[3] * 1 + sp_buffer[5] * 0;

        status_perfect_dribble_send = 1;

        break;
    }
    case 2: {
        motor_send[0] = 0;
        motor_send[1] = 0;

        if (force_dribble_control == 1 || force_dribble_control == 4) {
            initial_throttle[0] -= 0.5;
            initial_throttle[1] -= 0.5;
        } else if (force_dribble_control == 2) {
            sp_buffer[0] = -6000; // 1500 vs MR.DEV
            sp_buffer[1] = 6000; // 1500 vs MR.DEV

            initial_throttle[0] = throttle_norm[0];
            initial_throttle[1] = throttle_norm[1];
            break;
        }

        if (ros::Time::now().toSec() - last_timer > 0.7) {
            last_timer = ros::Time::now().toSec();
            dribble_up = 0; // TODO: ganti nanti ini hardcode
        }

        float final_sp[2];

        if (fabs(robot_vel_master[1]) > fabs(robot_vel_master[0]) * 0.1) {
            final_sp[0] = (float)90;
            final_sp[1] = (float)90;
        } else {
            final_sp[0] = (float)conf.perfect_backward_arm[0];
            final_sp[1] = (float)conf.perfect_backward_arm[1];
        }

        // final_sp[0] = (float)conf.perfect_backward_arm[0];
        // final_sp[1] = (float)conf.perfect_backward_arm[1];

        initial_throttle[0] = fmaxf((float)final_sp[0], fminf(throttle_norm[0], initial_throttle[0]));
        initial_throttle[1] = fmaxf((float)final_sp[1], fminf(throttle_norm[1], initial_throttle[1]));
        uint8_t used_sp[2];

        used_sp[0] = (uint8_t)initial_throttle[0];
        used_sp[1] = (uint8_t)initial_throttle[1];

        motor_send[0] = 0;
        motor_send[1] = 0;

        throttleControl(throttle_norm, used_sp, sp_buffer, conf.kPID_throttle_control);

        // if (v_robot[0] > 120)
        //     v_robot[0] = 120;
        // else if (v_robot[0] < -120)
        //     v_robot[0] = -120;

        // if (v_robot[1] > 120)
        //     v_robot[1] = 120;
        // else if (v_robot[1] < -120)
        //     v_robot[1] = -120;

        sp_buffer[4] = 0;
        sp_buffer[5] = 0;

        if (v_robot[0] > 5) {
            // sp_buffer[4] += (fmaxf(500, fabsf(v_robot[0]) * 36.5)); // kanan hisap dipercepat // bola lama 26.5
            // sp_buffer[5] += (fmaxf(500, fabsf(v_robot[0]) * 36.5));

            sp_buffer[4] += fabsf(v_robot[0]) * 36.5; // kanan hisap dipercepat // bola lama 26.5
            sp_buffer[5] += fabsf(v_robot[0]) * 36.5;
        }

        if (v_robot[0] < -5) {
            // sp_buffer[4] += -(fmaxf(500, fabsf(v_robot[0]) * 36.5)); // kanan hisap dipercepat // bola lama 26.5
            // sp_buffer[5] += -(fmaxf(500, fabsf(v_robot[0]) * 36.5));

            sp_buffer[4] += -fabsf(v_robot[0]) * 36.5; // kanan hisap dipercepat // bola lama 26.5
            sp_buffer[5] += -fabsf(v_robot[0]) * 36.5;
        }

        if (v_robot[1] > 5) {
            sp_buffer[4] += (fabsf(v_robot[1]) * 22);
            sp_buffer[5] -= (fabsf(v_robot[1]) * 22);
        }
        if (v_robot[1] < -5) {
            sp_buffer[4] -= (fabsf(v_robot[1]) * 30); // bola lama 28
            sp_buffer[5] += (fabsf(v_robot[1]) * 30);
        }

        // printf("spbuffer: %.2f %.2f\n", sp_buffer[4], sp_buffer[5]);

        sp_buffer[0] = sp_buffer[2] * 1 + sp_buffer[4] * 1;
        sp_buffer[1] = sp_buffer[3] * 1 + sp_buffer[5] * 1;

        // ROS_ERROR("Kontrol bola %d %d (%d %d) (%.2f %.2f) (i: %.2f %.2f) -> %.2f %.2f", used_sp[0], used_sp[1]
        //     , curr_gp_value[0], curr_gp_value[1],
        //     throttle_norm[0], throttle_norm[1]
        //     , integral_arm[0], integral_arm[1]
        //     , sp_buffer[0], sp_buffer[1]);

        if (fabs(throttle_norm[0] - conf.perfect_backward_arm[0]) < 20 && fabs(throttle_norm[1] - conf.perfect_backward_arm[1]) < 20) {
            counter_perfect++;
        }

        status_perfect_dribble_send = 1;

        // INGAF INGAT INI
        // sp_buffer[0] = -1000;
        // sp_buffer[1] = 1000;
        break;
    }
    }

    // if (counter_perfect > 2) {
    //     status_perfect_dribble_send = 1;
    // }

    prev_state_hysteresis = GET_FLAG(system_flags, HYSTERESIS);

    // =====================================================

    if (dribble_up) {
        uint8_t hardcode[2] = { 45, 60 };
        dribbleUpDownControl(throttle_norm, hardcode, pwm_dribble_aux);
    } else {
        pwm_dribble_aux[0] = 0;
        pwm_dribble_aux[1] = 0;
    }

    //============================================

    /* Convert sp_buffer that get from calculate (deg/100ms) to encoder unit (cnts/100ms) */
    float left_sp = sp_buffer[0] * DEG2CNTS * 0.01;
    float right_sp = sp_buffer[1] * DEG2CNTS * 0.01;

    /* PI Control system for controlling dribble velocity */
    {
        float pid_error[2];
        static float pid_prev_error[2];

        static float pid_proportional[2];
        static float pid_integral[2] = { 0, 0 };
        static float pid_derivative[2];

        float pid_output[2];

        pid_error[0] = left_sp - dribble_vx[0];
        pid_error[1] = right_sp - dribble_vx[1];

        for (int i = 0; i < 2; i++) {
            pid_proportional[i] = conf.kPID_dribble_vel[0] * pid_error[i];
            pid_integral[i] += conf.kPID_dribble_vel[1] * pid_error[i];
            pid_derivative[i] = conf.kPID_dribble_vel[2] * (pid_error[i] - pid_prev_error[i]);

            if (pid_integral[i] > MAXPWM)
                pid_integral[i] = MAXPWM;
            else if (pid_integral[i] < -MAXPWM)
                pid_integral[i] = -MAXPWM;

            pid_output[i] = pid_proportional[i] + pid_integral[i] + pid_derivative[i];

            // printf("Sebelum: %.2f ", pid_output[i]);
            if (pid_output[i] > MAXPWM)
                pid_output[i] = MAXPWM;
            else if (pid_output[i] < -MAXPWM)
                pid_output[i] = -MAXPWM;

            pid_prev_error[i] = pid_error[i];

            if (std::isnan(pid_output[i]))
                ros::shutdown();

            dribble_send[i] = (int16_t)pid_output[i];
        }

        if (dribble_send[1] == 0 || dribble_send[0] == 0) {
            // printf("state %d || sp %.2f %.2f || fb %d %d || kPID %.2f %.2f %.2f || gp % d % d || sp_conv % .2f % .2f || err % .2f % .2f || pid_kiri: %.2f %.2f %.2f -> %.2f || kanan: %.2f %.2f %.2f -> %.2f\n",
            //        state_control_dribble,
            //        sp_buffer[0], sp_buffer[1], dribble_vx[0], dribble_vx[1], conf.kPID_dribble_vel[0], conf.kPID_dribble_vel[1], conf.kPID_dribble_vel[2], curr_gp_value[0], curr_gp_value[1], left_sp, right_sp, pid_error[0], pid_error[1],
            //        pid_proportional[0], pid_integral[0], pid_derivative[0], pid_output[0],
            //        pid_proportional[1], pid_integral[1], pid_derivative[1], pid_output[1]);
        }
    }

    /* It will publish to STM node */
    std_msgs::Int16MultiArray msg_pub_vel_dribble;
    msg_pub_vel_dribble.data.push_back(dribble_send[1]);
    msg_pub_vel_dribble.data.push_back(dribble_send[0]);
    msg_pub_vel_dribble.data.push_back(kick_power_send * 100);
    msg_pub_vel_dribble.data.push_back(global_kicker_position);
    msg_pub_vel_dribble.data.push_back(throttle_norm[0]);
    msg_pub_vel_dribble.data.push_back(throttle_norm[1]);
    msg_pub_vel_dribble.data.push_back(0); // kick mode
    pub_to_stm.publish(msg_pub_vel_dribble);

    /* It will publish to motor node */
    std_msgs::Float32MultiArray msg_motor_send;
    msg_motor_send.data.push_back(motor_send[0]);
    msg_motor_send.data.push_back(motor_send[1]);
    pub_to_motor.publish(msg_motor_send);

    std_msgs::UInt8 msg_state_ballhandling;
    msg_state_ballhandling.data = state_control_dribble;
    pub_state_ballhandling.publish(msg_state_ballhandling);

    /* It will publish to master node */
    std_msgs::UInt8 msg_hysteresis_ball;
    msg_hysteresis_ball.data = COMPARE_BIT(system_flags, HYSTERESIS);
    pub_hysteresis_ball.publish(msg_hysteresis_ball);

    std_msgs::Int16MultiArray msg_pwm_dribble_aux;
    msg_pwm_dribble_aux.data.push_back(pwm_dribble_aux[0]);
    msg_pwm_dribble_aux.data.push_back(pwm_dribble_aux[1]);
    pub_pwm_dribble_aux.publish(msg_pwm_dribble_aux);
}

void callbackSubscribersub_v_robot_actuation(const geometry_msgs::Pose2DConstPtr& msg)
{
    v_robot[0] = msg->x;
    v_robot[1] = msg->y;
    v_robot[2] = msg->theta;

    v_robot[0] -= (v_robot[2] * DEG2RAD * 30);
}
