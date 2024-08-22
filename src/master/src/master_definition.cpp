#include "master/master.hpp"

// MachineState RoleAbstract::game_status = MachineState();
// ball_t RoleAbstract::ball = {0};
// pose2d_t RoleAbstract::final_pose = {0};

Master::Master(ros::NodeHandle* nh)
{
    pub_pc2stm = nh->advertise<iris_msgs::pc2stm>("/master/pc2stm", 1);
    pub_vel = nh->advertise<geometry_msgs::Pose2D>("/master/robot/vel", 1);
    pub_robot_condition = nh->advertise<std_msgs::Int16>("/master/robot/robot_condition", 1);
    pub_robot_offset = nh->advertise<geometry_msgs::Pose2D>("/world_model/robot/pose_offset", 1);
    pub_target_intercept = nh->advertise<geometry_msgs::Pose2D>("/master/ball/intercept", 1);
    pub_pose_used = nh->advertise<geometry_msgs::Pose2D>("/master/robot/pose_used", 1);
    pub_my_role = nh->advertise<std_msgs::UInt8>("/master/robot/role", 1);

    sub_bs2pc = nh->subscribe("/communication/icast/bs2pc", 1, &Master::callbackSubscribeBs2pc, this);
    // sub_obstacle = nh->subscribe("/world_model/obs/pos", 1, &Master::callbackSubscribeObstacle, this);
    sub_obstacle_pcl = nh->subscribe("/world_model/obs/pcl", 1, &Master::callbackSubscribeObstaclePCL, this);
    sub_robot_data = nh->subscribe("/communication/icast/data_robot", 1, &Master::callbackSubscribeRobotData, this);
    sub_velocity = nh->subscribe("/world_model/robot/vel", 1, &Master::callbackSubscribeVelocity, this);
    sub_final_pose = nh->subscribe("/world_model/robot/pose", 1, &Master::callbackSubscribeFinalPose, this);
    sub_final_ball = nh->subscribe("/world_model/ball/final_pos", 1, &Master::callbackSubscribeFinalBall, this);
    sub_final_ball_vel = nh->subscribe("/world_model/ball/final_vel", 1, &Master::callbackSubscribeFinalBallVel, this);
    sub_waypoints_ball = nh->subscribe("/world_model/ball/waypoints", 1, &Master::callbackSubscribeWaypointsBall, this);
    sub_ball_status = nh->subscribe("/world_model/ball/status", 1, &Master::callbackSubscribeBallStatus, this);
    sub_ball_status_vision = nh->subscribe("/world_model/ball/status", 1, &Master::callbackSubscribeBallStatusVision, this);
    sub_ball_status_hardware = nh->subscribe("/hardware/ballhandling/status", 1, &Master::callbackSubscribeBallStatusHardware, this);
    // sub_ball_status_hardware = nh->subscribe("/hardware/ballhandling/hysteresis", 1, &Master::callbackSubscribeBallStatusHardware, this);
    sub_stm2pc = nh->subscribe("/hardware/stm2pc", 1, &Master::callbackSubscribeStm2pc, this);
    sub_goalpost_data = nh->subscribe("/world_model/goalpost/data", 1, &Master::callbackSubscribeGoalpostData, this);
    sub_positioning = nh->subscribe("/world_model/positioning/titik_final_positioning", 1, &Master::callbackSubscribePositioning, this);

    tim_routine = nh->createTimer(ros::Duration(0.02), &Master::callbackRoutine, this);

    role_player[0] = std::make_unique<Goalkeeper>();
    role_player[1] = std::make_unique<Attacker>();
    role_player[2] = std::make_unique<Defender>();

    loadConfig();

    syncMemory();

    memset(&act_vel, 0, sizeof(act_vel));
    memset(&bs_data, 0, sizeof(bs_data));
    memset(&stm_data, 0, sizeof(stm_data));
    memset(&stm2pc_data, 0, sizeof(stm2pc_data));
    memset(&game_status, 0, sizeof(game_status));
    memset(&robot_condition, 0, sizeof(robot_condition));
    memset(&shared_robot_data, 0, sizeof(shared_robot_data));
    memset(&final_pose, 0, sizeof(final_pose));
    memset(&ball, 0, sizeof(ball));
    memset(&world_vel, 0, sizeof(world_vel));
    memset(&world_accel, 0, sizeof(world_accel));

    stm_data.global_kicker_position = 900;
    stm_data.enable_auto_correct_position = 1;
    stm_data.force_dribble_control = 0;

    last_time_stm_connected = ros::Time::now().toSec();

    robot_base_action = 1;
}

void Master::syncMemory()
{
    // Initialize Singleton
    motion = Motion::getInstance();
    motion->init(&final_pose, &act_vel, &world_vel, &ball, &stm_data, &ball_waypoints, &obstacle, &obstacle_pcl);

    motion->shared_robot_data[0] = &shared_robot_data[0];
    motion->shared_robot_data[1] = &shared_robot_data[1];
    motion->shared_robot_data[2] = &shared_robot_data[2];
    motion->shared_robot_data[3] = &shared_robot_data[3];
    motion->feedback_vel = &feedback_vel;
    motion->target_goal[0] = &target_goal[0];
    motion->target_goal[1] = &target_goal[1];
    motion->min_tolerance_empty_goal_on_field[0] = &min_tolerance_empty_goal_on_field[0];
    motion->min_tolerance_empty_goal_on_field[1] = &min_tolerance_empty_goal_on_field[1];
    motion->max_tolerance_empty_goal_on_field[0] = &max_tolerance_empty_goal_on_field[0];
    motion->max_tolerance_empty_goal_on_field[1] = &max_tolerance_empty_goal_on_field[1];
    motion->lock_goal_kiri_atau_kanan = &lock_goal_kiri_atau_kanan;
    motion->target_goal_vision = &target_goal_vision;
    motion->empty_goal_status = &empty_goal_status;
    motion->pose_used = &pose_used;

    for (uint8_t i = 0; i < 3; i++) {
        role_player[i]->final_pose = &final_pose;
        role_player[i]->ball = &ball;
        role_player[i]->game_status = &game_status;
        role_player[i]->bs_data = &bs_data;
        role_player[i]->stm_data = &stm_data;
        role_player[i]->motion = motion;
        role_player[i]->robot_condition = &robot_condition;
        role_player[i]->shared_robot_data[0] = &shared_robot_data[0];
        role_player[i]->shared_robot_data[1] = &shared_robot_data[1];
        role_player[i]->shared_robot_data[2] = &shared_robot_data[2];
        role_player[i]->shared_robot_data[3] = &shared_robot_data[3];

        role_player[i]->pos_buffer_robot_has_ball[0] = &pos_buffer_robot_has_ball[0];
        role_player[i]->pos_buffer_robot_has_ball[1] = &pos_buffer_robot_has_ball[1];
        role_player[i]->hisap_dribble_lama = &hisap_dribble_lama;
        role_player[i]->titik_final_positioning[0] = &titik_final_positioning[0];
        role_player[i]->titik_final_positioning[1] = &titik_final_positioning[1];

        role_player[i]->game_sub_status_prep = &game_sub_status_prep;
        role_player[i]->game_sub_status = &game_sub_status;

        role_player[i]->has_prep = &has_prep;

        role_player[i]->enemy_locked = &enemy_locked;
        role_player[i]->enemy_locked_obs = &enemy_locked_obs;
        role_player[i]->bola_terakhir_sebelum_hilang[0] = &bola_terakhir_sebelum_hilang[0];
        role_player[i]->bola_terakhir_sebelum_hilang[1] = &bola_terakhir_sebelum_hilang[1];
        role_player[i]->receive_lock = &receive_lock;

        role_player[i]->prev_ball[0] = &prev_ball[0];
        role_player[i]->prev_ball[1] = &prev_ball[1];
        role_player[i]->region_by_style = &region_by_style;

        role_player[i]->prev_n_has_ball = &prev_n_has_ball;

        role_player[i]->lock_goal_kiri_atau_kanan = &lock_goal_kiri_atau_kanan;
    }
}

Master::~Master()
{
}

void Master::callbackRoutine(const ros::TimerEvent& event)
{
    hardwareButtonHandler();

    static char* autorun = getenv("AUTORUN");
    if (autorun == NULL || atoi(autorun) == 0) {
        keyboardHandler();
    }

    BSCommandHandler();
    batteryChecker();

    // ROS_INFO("role %d ball %d %d", role, role_player[role]->ball->is_caught, ball.is_caught);
    // ROS_ERROR("ball %f %f", ball.pos.x, ball.pos.y);
    // ROS_WARN("nearest ball %f %f", nearest_ball.pos.x, nearest_ball.pos.y);

    /**
     * @brief Memastikan ketika bertanding, robot konek ke BS
     * @brief Jika tidak konek, maka robot tidak akan bergerak
     */
    game_status.value *= bs_data.is_iam_send_data_to_bs;

    try {
        /* code */
        playSelectorControl();
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    // ROS_INFO("GAMESTATUS: %d", game_status.value);

    // {
    //     shared_robot_data[2].pose.x = 50;
    //     shared_robot_data[2].pose.y = 100;

    //     obstacle_checking_t obs = motion->obstacleChecking(final_pose.theta, 60, 200, NORMAL_OBSTACLE_FRIEND);

    //     printf("obs: %d %.2f %.2f\n", obs.status, obs.pos_x, obs.pos_y);
    // }

    // {
    //     static uint16_t counter_masuk = 0;
    //     obstacle_checking_t obs_lawan = motion->obstacleCheckNearestTo(motion->RobotAngletoBall(), 25, motion->RobotDist2Point(ball.pos.x, ball.pos.y) + 40, ball.pos.x, ball.pos.y);

    //     float jarak = pythagoras(obs_lawan.pos_x, obs_lawan.pos_y, ball.pos.x, ball.pos.y);

    //     uint8_t bola_dipegang_lawan = 0;

    //     if (jarak < 25) {
    //         counter_masuk = fminf(50, counter_masuk + 1);
    //     } else {
    //         counter_masuk = fmaxf(0, counter_masuk - 2);
    //     }

    //     if (counter_masuk > 49) {
    //         bola_dipegang_lawan = 1;
    //     }

    //     // printf("%d %.2f %.2f || %.2f %.2f || %.2f\n", bola_dipegang_lawan, obs_lawan.pos_x, obs_lawan.pos_y, ball.pos.x, ball.pos.y, jarak);
    // }

    transmitAll();
}

void Master::callbackSubscribeFinalPose(const geometry_msgs::Pose2DConstPtr& msg)
{
    final_pose.x = msg->x;
    final_pose.y = msg->y;
    final_pose.theta = msg->theta;
}
void Master::callbackSubscribeFinalBall(const geometry_msgs::Pose2DConstPtr& msg)
{
    if (msg->x > FLT_EPSILON || msg->y > FLT_EPSILON || msg->x < -FLT_EPSILON || msg->y < -FLT_EPSILON) {
        ball.pos.x = msg->x;
        ball.pos.y = msg->y;
        ball.angle = msg->theta;
        ball.distance = pythagoras(final_pose.x, final_pose.y, ball.pos.x, ball.pos.y);
    }
}
void Master::callbackSubscribeFinalBallVel(const geometry_msgs::Pose2DConstPtr& msg)
{
    ball.vel.x = msg->x;
    ball.vel.y = msg->y;
}
void Master::callbackSubscribeWaypointsBall(const sensor_msgs::PointCloudConstPtr& msg)
{
    mutex_waypoint_ball.lock();
    ball_waypoints.clear();
    size_t wp_size = msg->points.size();
    for (size_t i = 0; i < wp_size; i++) {
        point2d_t temp;
        temp.x = msg->points[i].x;
        temp.y = msg->points[i].y;
        ball_waypoints.push_back(temp);
    }
    mutex_waypoint_ball.unlock();
}
void Master::callbackSubscribeBallStatus(const std_msgs::UInt8ConstPtr& msg)
{
    ball.status = msg->data;
}
void Master::callbackSubscribeBallStatusVision(const std_msgs::UInt8ConstPtr& msg)
{
    ball.is_visible = msg->data;
}
void Master::callbackSubscribeBallStatusHardware(const std_msgs::UInt8ConstPtr& msg)
{
    ball.is_caught = msg->data;
}
void Master::callbackSubscribeObstacle(const sensor_msgs::PointCloudConstPtr& msg)
{
    obstacle.clear();
    size_t obs_size = msg->points.size();
    for (size_t i = 0; i < obs_size; i++) {
        point2d_t temp;
        temp.x = msg->points[i].x;
        temp.y = msg->points[i].y;
        obstacle.push_back(temp);
    }
}

void Master::callbackSubscribeObstaclePCL(const sensor_msgs::PointCloudConstPtr& msg)
{
    mutex_obs.lock();
    obstacle_pcl.clear();
    for (size_t i = 0; i < msg->points.size(); i++) {
        point2d_t temp_pcl;
        temp_pcl.x = msg->points[i].x;
        temp_pcl.y = msg->points[i].y;
        obstacle_pcl.push_back(temp_pcl);
    }
    mutex_obs.unlock();
}

void Master::callbackSubscribeGoalpostData(const iris_msgs::goalpostConstPtr& msg)
{
    /* Get enemy keeper position based on field */
    keeper_on_field[0] = msg->keeper_on_field_x;
    keeper_on_field[1] = msg->keeper_on_field_y;

    // EMPTY GOAL TRACKING
    empty_goal_status = msg->empty_goal_status;
    if (empty_goal_status) {
        empty_goal_on_field[0] = msg->empty_goal_field_x;
        empty_goal_on_field[1] = msg->empty_goal_field_y;
        empty_goal_on_field[2] = msg->empty_goal_field_theta;

        empty_goal_on_frame[0] = msg->empty_goal_frame_x;
        empty_goal_on_frame[1] = msg->empty_goal_frame_y;
        empty_goal_on_frame[2] = msg->empty_goal_frame_theta;

        max_tolerance_empty_goal_on_field[0] = msg->empty_goal_max_tolerance_x;
        max_tolerance_empty_goal_on_field[1] = msg->empty_goal_max_tolerance_y;
        max_tolerance_empty_goal_on_field[2] = angleToTarget(max_tolerance_empty_goal_on_field[0], max_tolerance_empty_goal_on_field[1], final_pose);

        min_tolerance_empty_goal_on_field[0] = msg->empty_goal_min_tolerance_x;
        min_tolerance_empty_goal_on_field[1] = msg->empty_goal_min_tolerance_y;
        min_tolerance_empty_goal_on_field[2] = angleToTarget(min_tolerance_empty_goal_on_field[0], min_tolerance_empty_goal_on_field[1], final_pose);

        target_goal_vision = msg->sudut_robot_ke_gawang_vision;
        lock_goal_kiri_atau_kanan = msg->lock_goal_kiri_atau_kanan;

        float gawang_x_cm, gawang_y_cm;
        float robot_x_cm, robot_y_cm;

        gawang_x_cm = empty_goal_on_field[0];
        gawang_y_cm = empty_goal_on_field[1];
        robot_x_cm = final_pose.x;
        robot_y_cm = final_pose.y;

        float dx = gawang_x_cm - robot_x_cm;
        float dy = gawang_y_cm - robot_y_cm;
        float m = tanf(atan2f(dy, dx));
        float c = robot_y_cm - m * robot_x_cm;

        target_goal[1] = 1200;
        target_goal[0] = (target_goal[1] - c) / m;
    } else {
        empty_goal_on_field[0] = 400;
        empty_goal_on_field[1] = 1200;
        empty_goal_on_field[2] = angleToTarget(400, 1200, final_pose);

        target_goal[0] = 400;
        target_goal[1] = 1200;
    }

    // printf("TARGET GOAL : %.2f %.2f\n", target_goal[0], target_goal[1]);
}

void Master::callbackSubscribePositioning(const std_msgs::Float32MultiArrayConstPtr& msg)
{
    titik_final_positioning[0] = msg->data.at(0);
    titik_final_positioning[1] = msg->data.at(1);
}

void Master::callbackSubscribeBs2pc(const iris_msgs::bs2pcConstPtr& msg)
{
    bs_data.is_iam_controlled_by_bs = (msg->data_mux_control & (robot_num_bin[robot_num])) >> (robot_num - 1);
    // printf("%d %d\n", msg->data_mux_control, (robot_num_bin[robot_num]));

    // printf("RAW: %d %d %d\n", bs_data.is_iam_controlled_by_bs, bs_data.is_iam_send_data_to_bs, msg->command);

    if (bs_data.is_iam_controlled_by_bs && bs_data.is_iam_send_data_to_bs) {
        bs_data.last_time_connected = ros::Time::now().toSec(); // Update BS last time connected

        bs_data.command = msg->command;
        bs_data.mode = msg->mode;
        bs_data.style = msg->style;

        bs_data.obs_bs_regional[0] = msg->index_obs_keeper;
        bs_data.obs_bs_regional[1] = msg->index_obs_center;
        bs_data.obs_bs_regional[2] = msg->index_obs_side;

        // printf("command:%d style:%d mode:%d \n", bs_data.command, bs_data.style, bs_data.mode);

        readU16Bit(&bs_data.n_active_robot, &msg->data_mux_1, 3, 0);
        readU16Bit(&bs_data.n_nearest_ball, &msg->data_mux_1, 3, 3);
        readU16Bit(&bs_data.n_has_ball, &msg->data_mux_1, 3, 6);
        readU16Bit(&bs_data.n_pass, &msg->data_mux_1, 3, 9);
        readU16Bit(&bs_data.n_pass_target, &msg->data_mux_1, 3, 12);

        // printf("active robot: %d nearest ball: %d has ball: %d pass: %d pass target: %d \n", bs_data.n_active_robot, bs_data.n_nearest_ball, bs_data.n_has_ball, bs_data.n_pass, bs_data.n_pass_target);

        readU16Bit(&shared_robot_data[1].role, &msg->data_mux_2, 3, 0);
        readU16Bit(&shared_robot_data[2].role, &msg->data_mux_2, 3, 3);
        readU16Bit(&shared_robot_data[3].role, &msg->data_mux_2, 3, 6);
        readU16Bit(&shared_robot_data[4].role, &msg->data_mux_2, 3, 9);
        readU16Bit(&shared_robot_data[5].role, &msg->data_mux_2, 3, 12);

        bs_data.trim_vel_linear = msg->trim_translation_vel;
        bs_data.trim_vel_angular = msg->trim_rotation_vel;
        bs_data.trim_kick_power = msg->trim_kick_power;

        role = shared_robot_data[robot_num].role;
        for (int8_t i = 1; i < 4; i++) {
            switch (shared_robot_data[i].role) {
            case 1:
                bs_data.n_robot_att = i;
                break;
            case 2:
                bs_data.n_robot_def_1 = i;
                break;
            case 3:
                bs_data.n_robot_assist = i;
                break;
            case 4:
                bs_data.n_robot_def_2 = i;
                break;
            case 0:
                bs_data.n_robot_gk = 0;
                break;
            }
        }
    }

    // LogWithColor("green", " ROLE: %d\n", role);
}

void Master::callbackSubscribeStm2pc(const iris_msgs::stm2pcConstPtr& msg)
{
    last_time_stm_connected = ros::Time::now().toSec();
    /* buttons (bit-selection) (active low) */
    /**
     * button_1= 0b11111110;
     * button_2= 0b11111101;
     * button_3= 0b11111011;
     * button_4= 0b11110111;
     * button_5= 0b11101111;
     * toggle_1= 0b11011111;
     * toggle_2= 0b10111111;
     * toggle_3= 0b01111111;
     */
    stm2pc_data.button1_pressed = !((msg->buttons & 0b00000001) >> 0);
    stm2pc_data.button2_pressed = !((msg->buttons & 0b00000010) >> 0x01);
    stm2pc_data.button3_pressed = !((msg->buttons & 0b00000100) >> 0x02);
    stm2pc_data.button4_pressed = !((msg->buttons & 0b00001000) >> 0x03);
    stm2pc_data.button5_pressed = !((msg->buttons & 0b00010000) >> 0x04);
    stm2pc_data.toggle1_pressed = !((msg->buttons & 0b00100000) >> 0x05);
    stm2pc_data.toggle2_pressed = !((msg->buttons & 0b01000000) >> 0x06);
    stm2pc_data.toggle3_pressed = !((msg->buttons & 0b10000000) >> 0x07);
    stm2pc_data.battery = msg->battery;

// #ifndef DEBUG
// #define DEBUG
// #endif
#ifdef DEBUG
    printf("stm_data: %d %d %d %d %d %d %d %d %.2f\n",
        stm2pc_data.button1_pressed, stm2pc_data.button2_pressed,
        stm2pc_data.button3_pressed, stm2pc_data.button4_pressed,
        stm2pc_data.button5_pressed, stm2pc_data.toggle1_pressed,
        stm2pc_data.toggle2_pressed, stm2pc_data.toggle3_pressed,
        stm2pc_data.battery);
#else
    if (!bs_data.is_iam_send_data_to_bs) {
        ROS_WARN("Master : %d Not Connected to BS", bs_data.is_iam_send_data_to_bs);
    }
#endif

    /* last time connected */
    stm2pc_data.last_time_connected = ros::Time::now().toSec();
}

void Master::callbackSubscribeRobotData(const iris_msgs::data_robotsConstPtr& msg)
{
    for (uint8_t i = 1; i < msg->pose_x.size(); i++) {
        shared_robot_data[i].pose.x = msg->pose_x.at(i);
    }
    for (uint8_t i = 1; i < msg->pose_y.size(); i++) {
        shared_robot_data[i].pose.y = msg->pose_y.at(i);
    }
    for (uint8_t i = 1; i < msg->pose_theta.size(); i++) {
        shared_robot_data[i].pose.theta = msg->pose_theta.at(i);
    }
    for (uint8_t i = 1; i < msg->robot_condition.size(); i++) {
        shared_robot_data[i].robot_condition = msg->robot_condition.at(i);
    }
    for (uint8_t i = 1; i < msg->target_positioning_x.size(); i++) {
        shared_robot_data[i].target_positioning[0] = msg->target_positioning_x.at(i);
        shared_robot_data[i].target_positioning[1] = msg->target_positioning_x.at(i);
    }
}

void Master::hardwareButtonHandler()
{
    if (stm2pc_data.button1_pressed == 1) { // BUG: FIX THIS
        ROS_WARN("Master : Button 1 Pressed -> Kicking Ball");
        kickerControl(&stm_data, NO_AIM, 7);
    }

    if (stm2pc_data.button2_pressed == 1) {
        ROS_WARN("Master : Button 2 Pressed -> Kicking Ball");
        stm_data.global_kicker_position = 500;
    }

    if (stm2pc_data.button3_pressed) {
        setOffset(530, 600, 90);
        game_status.value = 0;
        role_player[role]->game_sub_status_prep->value = 0;
        role_player[role]->game_sub_status->value = 0;
        buzzerControl(&stm_data, 10, 20);
    }

    if (stm2pc_data.button4_pressed) {
    }

    if (stm2pc_data.button5_pressed) {
        setOffset(init_offset.x, init_offset.y, init_offset.theta);
        game_status.value = 0;
        role_player[role]->game_sub_status_prep->value = 0;
        role_player[role]->game_sub_status->value = 0;
        buzzerControl(&stm_data, 10, 10);
    }

    if (stm2pc_data.toggle1_pressed) {
    }

    if (stm2pc_data.toggle2_pressed) {
        bs_data.is_iam_send_data_to_bs = 1;
    } else {
        bs_data.is_iam_send_data_to_bs = 0;
    }

    bs_data.is_iam_send_data_to_bs = 1;

    if (stm2pc_data.toggle3_pressed) {
    }
}

void Master::keyboardHandler()
{
    if (kbhit() > 0) {
        char key = std::cin.get();

        switch (key) {
        case '*':
            stm_data.global_kicker_position = 500;
            break;
        case '}':
            robot_condition = 0;
            break;
        case '{':
            robot_condition = 11;
            break;
        case 'j':
            game_status.value = 'j';
            break;
        case 'b':
            game_status.value = 'b';
            break;
        case 'n':
            game_status.value = 'n';
            break;
        case 'm':
            game_status.value = 'm';
            break;
        case '0':
            game_status.value = '0';
            break;
        case '9':
            game_status.value = '9';
            break;
        case 'z':
            game_status.value = 'z';
            break;
        case ';':
            kickerControl(&stm_data, NO_AIM, 8, 400);
            break;
        case ':':
            kickerControl(&stm_data, OFFBALLWITHDRIBBLE, 6, 1000); // 1000 : datar ; 200 : lambung
            break;
        case '-':
            kickerControl(&stm_data, ONLY_DISCHARGE);
            break;
        case '+':
            kickerControl(&stm_data, ONLY_CHARGE);
            break;
        case 'o':
            setOffset(init_offset.x, init_offset.y, init_offset.theta);
            buzzerControl(&stm_data, 10, 10);
            break;
        case ',':
            robot_condition = 0; //
            // stm_data.force_dribble_control = 0;
            break;
        case '.':
            robot_condition = 11;
            stm_data.force_dribble_control = 1;
            break;
        case '/':
            stm_data.force_dribble_control = 2;
            break;
        case '1':
            role = ROLE_ATTACKER;
            break;
        case '2':
            role = ROLE_DEFENDER;
            break;
        case 'q':
            bs_data.style = 65;
            break;
        case 'w':
            bs_data.style = 66;
            break;
        case 'e':
            bs_data.style = 67;
            break;
        case 'r':
            bs_data.style = 68;
            break;
        case '5':
            bs_data.style = 69;
            break;

        // Penting
        case 'K':
            game_status.value = 'K';
            break;
        case 'k':
            game_status.value = 'k';
            break;
        case 's':
            game_status.value = game_status.value + 128;
            break;
        case ' ':
            game_status.value = ' ';
            break;
        case 'l':
            kickerControl(&stm_data, OFFBALLWITHDRIBBLE, 2);
            break;
        case 'L':
            game_status.value = status_park;
            break;
        case 'F':
            game_status.value = status_preparation_freekick_home;
            break;
        case 'C':
            game_status.value = status_preparation_cornerkick_home;
            break;
        case 'G':
            game_status.value = status_preparation_goalkick_home;
            break;
        case 'P':
            game_status.value = status_preparation_penaltykick_home;
            break;
        case 'T':
            game_status.value = status_preparation_throwin_home;
            break;
        case 'N':
            game_status.value = status_preparation_dropball;
            break;

        case 'f':
            game_status.value = status_preparation_freekick_away;
            break;
        case 'c':
            game_status.value = status_preparation_cornerkick_away;
            break;
        case 'g':
            game_status.value = status_preparation_goalkick_away;
            break;
        case 'p':
            game_status.value = status_preparation_penaltykick_away;
            break;
        case 't':
            game_status.value = status_preparation_throwin_away;
            break;
        case ']':
            game_status.value = ']';
            break;
        case '[':
            game_status.value = '[';
            break;
        case '|':
            game_status.value = '|';
            break;
        case 'x':
            game_status.value = 'x';
            break;
        case 'v':
            game_status.value = 'v';
            break;
        case 'd':
            game_status.value = 'd';
            break;
        default:
            break;
        }

        // printf("game_status: %d\n", game_status.value);

        robot_base_action = (key != 'S' && key != ' ');
    }
}

void Master::BSCommandHandler()
{
    static uint8_t prev_BS_cmd = 0;

    if (bs_data.is_iam_controlled_by_bs) {
        // printf("cok: %d\n", bs_data.command);
        if (prev_BS_cmd != bs_data.command) {
            if (bs_data.command == 's') {
                if (game_status.value > 0 && game_status.value <= 127)
                    game_status.value += 128;
            } else if (bs_data.command == 'S') {
                // game_status.value = ' ';
                if (game_status.value > 127 && game_status.value <= 255)
                    game_status.value -= 128;
            } else if (bs_data.command != 126) {
                game_status.value = bs_data.command;
            }

            robot_base_action = (!(prev_BS_cmd == 126 && bs_data.command == 'S') && bs_data.command != ' ') * (bs_data.command != 126) | (bs_data.command == status_park);
        }
        prev_BS_cmd = bs_data.command;
    } else {
        prev_BS_cmd = 126;
    }

    // printf("Hasil: %d %d\n", game_status.value, robot_base_action);
}

void Master::batteryChecker()
{
    static double last_time_buzzer_called = 0;
    if (stm2pc_data.battery < 36.5 && stm2pc_data.battery > 0 && ros::Time::now().toSec() - last_time_buzzer_called > 1) {
        ROS_ERROR("BATTERY LOW : %.2f", stm2pc_data.battery);
        buzzerControl(&stm_data, 10, 10);
        last_time_buzzer_called = ros::Time::now().toSec();
    }
}

void Master::transmitAll()
{
    //-- Kicker Handler to make sure data is not streamed
    static double last_time_kick = 0;
    static double start_time_kick = 0;
    static uint8_t prev_kicker_mode = 0;

    if (stm_data.kicker_mode != 0 && prev_kicker_mode == 0)
        start_time_kick = ros::Time::now().toSec();

    if (stm_data.kicker_mode != 0)
        last_time_kick = ros::Time::now().toSec();

    if (last_time_kick - start_time_kick > 1.5)
        stm_data.kicker_mode = 0;

    if (ball.is_caught == 0)
        stm_data.global_kicker_position = 800;

    prev_kicker_mode = stm_data.kicker_mode;

    static uint16_t counter_2 = 0;

    if (ball.is_caught > 0)
        counter_2++;
    else
        counter_2 = 0;

    // if (counter_2 > 50)
    //     stm_data.force_dribble_control = 1;

    iris_msgs::pc2stm pc2stm_msg;
    pc2stm_msg.kicker_mode = stm_data.kicker_mode;
    pc2stm_msg.kicker_power = stm_data.kicker_power;
    pc2stm_msg.kicker_position = stm_data.kicker_position;
    pc2stm_msg.buzzer_cnt = stm_data.buzzer_cnt;
    pc2stm_msg.buzzer_time = stm_data.buzzer_time;
    pc2stm_msg.dribble_up = stm_data.dribble_up;
    pc2stm_msg.enable_auto_correct_position = stm_data.enable_auto_correct_position;
    pc2stm_msg.force_dribble_control = stm_data.force_dribble_control;
    pc2stm_msg.long_dribble_control = stm_data.long_dribble_control;
    pc2stm_msg.global_kicker_position = stm_data.global_kicker_position;
    pc2stm_msg.stand_by = stm_data.stand_by;
    pub_pc2stm.publish(pc2stm_msg);

    geometry_msgs::Pose2D vel_msg;

    if (ros::Time::now().toSec() - last_time_stm_connected > 0.04) {
        act_vel.x = 0;
        act_vel.y = 0;
        act_vel.theta = 0;
    } else {
        if (bs_data.is_iam_controlled_by_bs) {
            vel_msg.x = act_vel.x * bs_data.trim_vel_linear / 25.0f;
            vel_msg.y = act_vel.y * bs_data.trim_vel_linear / 25.0f;
            vel_msg.theta = act_vel.theta * bs_data.trim_vel_angular / 10.0f;
        } else {
            vel_msg.x = act_vel.x;
            vel_msg.y = act_vel.y;
            vel_msg.theta = act_vel.theta;
        }
    }
    pub_vel.publish(vel_msg);

    // robot_condition = 11;

    std_msgs::Int16 robot_condition_msg;
    robot_condition_msg.data = robot_condition;
    pub_robot_condition.publish(robot_condition_msg);

    /* This line is to reset state, cause of STM is using trigger-logic instead of streaming data */
    if ((stm_data.buzzer_time != 0) || (stm_data.buzzer_cnt != 0)) {
        stm_data.buzzer_time = 0;
        stm_data.buzzer_cnt = 0;
    }

    geometry_msgs::Pose2D target_intercept_msg;
    target_intercept_msg.x = motion->target_intercept_ball_x;
    target_intercept_msg.y = motion->target_intercept_ball_y;
    pub_target_intercept.publish(target_intercept_msg);

    std_msgs::UInt8 msg_my_role;
    msg_my_role.data = role;
    pub_my_role.publish(msg_my_role);

    geometry_msgs::Pose2D pose_used_msg;
    pose_used_msg.x = pose_used.x;
    pose_used_msg.y = pose_used.y;
    pose_used_msg.theta = pose_used.theta;
    pub_pose_used.publish(pose_used_msg);
}

void Master::playSelectorControl()
{
    static int16_t prev_game_status = 0;
    static int16_t curr_game_status = 0;
    static uint8_t prev_n_active_robot = bs_data.n_active_robot;
    static uint8_t prev_style = bs_data.style;
    static uint8_t prev_role = role;
    static uint8_t curr_role = role;

    curr_game_status = game_status.value;
    curr_role = role;

    role_player[role]->game_sub_status_prep->value *= (curr_game_status == prev_game_status);
    role_player[role]->game_sub_status->value *= (curr_game_status == prev_game_status);

    role_player[role]->game_sub_status_prep->value *= (robot_base_action != 0);
    role_player[role]->game_sub_status->value *= (robot_base_action != 0);

    role_player[role]->game_sub_status_prep->value *= (bs_data.n_active_robot == prev_n_active_robot);
    role_player[role]->game_sub_status->value *= (bs_data.n_active_robot == prev_n_active_robot);

    role_player[role]->game_sub_status_prep->value *= (bs_data.style == prev_style);
    role_player[role]->game_sub_status->value *= (bs_data.style == prev_style);

    robot_condition *= (curr_game_status == prev_game_status);
    robot_condition *= (curr_role == prev_role);

    prev_game_status = curr_game_status;
    prev_n_active_robot = bs_data.n_active_robot;
    prev_style = bs_data.style;
    prev_role = curr_role;

    if (game_status.value > 66 && game_status.value < 85 && game_status.value != 78 && game_status.value != 80) // Prep homes
    {
        has_prep = 0;
    } else if (game_status.value > 98 && game_status.value < 117 && game_status.value != 112) // Prep away
    {
        has_prep = 2;
    } else if (game_status.value == 78) // dropball
    {
        has_prep = 1;
    } else if (game_status.value == status_preparation_penaltykick_home) {
        has_prep = 3;
    } else if (game_status.value == status_preparation_penaltykick_away) {
        has_prep = 4;
    }

    // printf("status game: %d %d\n", game_status.value, robot_base_action);

#ifndef NASIONAL
#define NASIONAL
#endif
    switch (game_status.value * robot_base_action) {
    case 0:
        motion->manualMotion(0, 0, 0);
        stm_data.stand_by = 0;
        // stm_data.force_dribble_control = 1;
        break;
#ifndef NASIONAL
    case status_preparation_cornerkick_home:
    case status_preparation_freekick_home:
    case status_preparation_goalkick_home:
    case status_preparation_kickoff_home:
    case status_preparation_penaltykick_home:
    case status_preparation_throwin_home:
        role_player[role]->preparationGlobal();
        break;
#else
    case status_preparation_kickoff_home:
        if (bs_data.n_active_robot > 1) {
            if (role == 2) {
                role_player[role]->PrepKickoffHomeDouble();
            } else {
                role_player[role]->PrepKickoffHomeDouble();
            }
        } else {
            role_player[role]->PrepKickoffHomeSingle();
        }
        break;
    case status_preparation_cornerkick_home:
        if (bs_data.n_active_robot > 1) {
            if (role == 2) {
                role_player[role]->PrepCornerHomeDouble();
            } else {
                role_player[role]->PrepparationGlobalDynamic();
            }
        } else {
            role_player[role]->PrepCornerHomeSingle();
        }
        break;
    case status_preparation_freekick_home:
        if (bs_data.n_active_robot > 1) {
            if (role == 2) {
                role_player[role]->PrepFreekickHomeDouble();
            } else {
                role_player[role]->PrepparationGlobalDynamic();
            }
        } else {
            role_player[role]->PrepFreekickHomeSingle();
        }
        break;
    case status_preparation_goalkick_home:
        if (bs_data.n_active_robot > 1) {
            if (role == 2) {
                role_player[role]->PrepGoalkickHomeDouble();
            } else {
                role_player[role]->PrepparationGlobalDynamic();
            }
        } else {
            role_player[role]->PrepGoalkickHomeSingle();
        }
        break;
    case status_preparation_penaltykick_home:
        if (bs_data.n_active_robot > 1) {
            role_player[role]->PrepPenaltyHomeDouble();
        } else {
            role_player[role]->PrepPenaltyHomeSingle();
        }
        break;
    case status_preparation_throwin_home:
        if (bs_data.n_active_robot > 1) {
            if (role == 2) {
                role_player[role]->PrepThrowinHomeDouble();
            } else {
                role_player[role]->PrepparationGlobalDynamic();
            }
        } else {
            role_player[role]->PrepThrowinHomeSingle();
        }
        break;
    case status_preparation_dropball:
        if (bs_data.n_active_robot <= 1) {
            role_player[role]->PrepDropBallSingle();
        } else {
            role_player[role]->PrepDropBallDouble();
        }
        break;

    case status_preparation_kickoff_away:
        if (bs_data.n_active_robot > 1) {
            if (role == 2) {
                role_player[role]->PrepKickoffAwayDouble();
            } else {
                role_player[role]->PrepKickoffAwayDouble();
            }
        } else {
            role_player[role]->PrepKickoffAwaySingle();
        }
        break;
    case status_preparation_cornerkick_away:
        if (bs_data.n_active_robot > 1) {
            if (role == 2) {
                role_player[role]->PrepCornerAwayDouble();
            } else {
                role_player[role]->PrepCornerAwayDouble();
            }
        } else {
            role_player[role]->PrepCornerAwaySingle();
        }
        break;
    case status_preparation_freekick_away:
        if (bs_data.n_active_robot > 1) {
            if (role == 2) {
                role_player[role]->PrepFreekickAwayDouble();
            } else {
                role_player[role]->PrepFreekickAwayDouble();
            }
        } else {
            role_player[role]->PrepFreekickAwaySingle();
        }
        break;
    case status_preparation_goalkick_away:
        if (bs_data.n_active_robot > 1) {
            if (role == 2) {
                role_player[role]->PrepGoalkickAwayDouble();
            } else {
                role_player[role]->PrepGoalkickAwayDouble();
            }
        } else {
            role_player[role]->PrepGoalkickAwaySingle();
        }
        break;
    case status_preparation_throwin_away:
        if (bs_data.n_active_robot > 1) {
            if (role == 2) {
                role_player[role]->PrepThrowinAwayDouble();
            } else {
                role_player[role]->PrepThrowinAwayDouble();
            }
        } else {
            role_player[role]->PrepThrowinAwaySingle();
        }
        break;
    case status_preparation_penaltykick_away:
        if (bs_data.n_active_robot > 1) {
            role_player[role]->PrepPenaltyAwayDouble();
        } else {
            role_player[role]->PrepPenaltyAwaySingle();
        }

        break;
#endif

    case game_kickoff_home:
    case game_cornerkick_home:
    case game_freekick_home:
    case game_goalkick_home:
    case game_penaltykick_home:
    case game_throwin_home:
    case game_kickoff_away:
    case game_cornerkick_away:
    case game_freekick_away:
    case game_goalkick_away:
    case game_penaltykick_away:
    case game_throwin_away:
    case game_dropball:
#ifndef NASIONAL
        switch (bs_data.style) {
        case 'A':
            role_player[role]->gameStyleA();
            break;
        case 'B':
            role_player[role]->gameStyleB();
            break;
        case 'C':
            role_player[role]->gameStyleC();
            break;
        case 'D':
            role_player[role]->gameStyleD();
            break;
        case 'E':
            role_player[role]->gameStyleE();
            break;
        }
        break;
#else
        switch (bs_data.n_active_robot) {
        case 0:
            role_player[role]->GameMainSingle();
            break;
        case 1:
            role_player[role]->GameMainSingle();
            break;
        case 2:
            role_player[role]->GameMainDouble();
            break;
        default:
            role_player[role]->GameMainDouble();
            break;
        }
        break;
#endif

    case status_park:
        motion->motionToPoint(init_offset.x, init_offset.y, init_offset.theta, 150, 180, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | NORMAL_OBSTACLE_FRIEND);
        break;
    //-------------------------------------------------------------------
    case 'j':
        motion->manualMotion(0, 150, 0);
        break;
    case 'b':
        motion->manualMotion(-150, 0, 0);
        break;
    case 'n':
        motion->manualMotion(0, -150, 0);
        break;
    case 'm':
        motion->manualMotion(150, 0, 0);
        break;
    case '0':
        motion->manualMotion(0, 0, -120);
        break;
    case '9':
        motion->manualMotion(0, 0, 120);
        break;
    //----------------------------------------------------------------------
    case status_idle_2:
        motion->manualMotion(0, 0, 0);
        break;
    case 'z':
        // motion->motionAroundPointAngle(FIELD_X_1_2, FIELD_Y_1_2, 160, angleToTarget(FIELD_X_1_2, FIELD_Y_1_2, final_pose), 275, 170, 90, NORMAL_OBSTACLE);
        // if (motion->ruckigToPoint(400, 600, 180, 200, 120, NORMAL_OBSTACLE | INVERT_OBS)) {
        //     motion->manualMotion(0, 0, 0);
        // } // game_status.value = 'x';

        if (motion->hadapGawangVision(0, 0, 120)) {
            // motion->manualMotion(0, 0, 0);
        }

        // if (motion->motionAroundBall(90, 90, 120, 120, 90))
        // {
        //     motion->manualMotion(0, 0, 0);
        // }
        break;
    case 'x':
        if (motion->receiveBall3(0, 0)) {
            motion->manualMotion(0, 0, 0);
        }

        // if (motion->motionAroundBall(90, 90, 210, 120, 90))
        // {
        //     motion->manualMotion(0, 0, 0);
        // }
        break;
    case 'd':
        if (motion->motionAroundBall(90, 90, 285, 120, 90)) {
            motion->manualMotion(0, 0, 0);
        }
        break;
    case ']':
        if (motion->catchingBall(200, 180)) {
            motion->manualMotion(0, 0, 0);
        }
        break;
    case '[':
        if (motion->catchingBall(200, 180, LANGSUNG_AMBIL)) {
            motion->manualMotion(0, 0, 0);
        }
        break;
    case '|':
        motion->newMotionToPoint(400, 400, 180, 80, 50);
        break;

    case '}':
        if (motion->motionToPoint(500, 180, 90, 250, 100, NORMAL_OBSTACLE | PREDICT))
            game_status.value = '{';
        break;

    case '{':
        if (motion->motionToPoint(500, 600, 90, 250, 100, NORMAL_OBSTACLE | PREDICT))
            game_status.value = '}';
        break;
    case 'v':
        // role_player[role]->PrepFreekickHomeSingle();
        // motion->motionAroundBall(atan2(ball.pos.y - 1200, ball.pos.x - 0) RAD2DEG, atan2(ball.pos.y - 1200, ball.pos.x - 0) RAD2DEG - 180, 70, 100, 70, NORMAL_OBSTACLE);
        break;
    }

    bs_data.prev_style = bs_data.style;
}

void Master::callbackSubscribeVelocity(const geometry_msgs::Pose2DConstPtr& msg)
{
    feedback_vel.x = msg->x;
    feedback_vel.y = msg->y;
    feedback_vel.theta = msg->theta;
}

void Master::setOffset(float pose_x, float pose_y, float pose_theta)
{
    geometry_msgs::Pose2D offset_msg;
    offset_msg.x = pose_x;
    offset_msg.y = pose_y;
    offset_msg.theta = pose_theta;
    pub_robot_offset.publish(offset_msg);
}

void Master::loadConfig()
{
    robot_num = atoi(getenv("ROBOT"));

    char config_file[100], config_file2[100];
    std::string current_dir = ros::package::getPath("master");
    sprintf(config_file, "%s/../../config/IRIS%d/static_conf.yaml", current_dir.c_str(), robot_num);
    printf("MASTER LOAD: %s\n", config_file);

    YAML::Node config = YAML::LoadFile(config_file);
    init_offset.x = config["Init"]["pose_x"].as<float>();
    init_offset.y = config["Init"]["pose_y"].as<float>();
    init_offset.theta = config["Init"]["pose_th"].as<float>();
    role = config["Init"]["role"].as<int>();
}