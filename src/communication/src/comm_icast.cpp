#include "communication/comm_icast.hpp"

//asdasdasdas
int main(int argc, char** argv)
{
    ros::init(argc, argv, "comm_icast");
    ros::NodeHandle nh;
    ros::MultiThreadedSpinner spinner(0);

    std::string config_path = ros::package::getPath("communication");
    size_t found = config_path.find("/src/communication");
    if (found != std::string::npos) {
        config_path.erase(found);
    }

    icast->setMcLoopback(0);
    icast->init(config_path);
    epoch.data = 0;

    if (!icast->mc->initialized()) {
        ROS_ERROR("ICAST ERROR : Failed to initialize the ICAST");
        ros::shutdown();
        return 0;
    }

    pub_bs2pc = nh.advertise<iris_msgs::bs2pc>("/communication/icast/bs2pc", 1);
    pub_data_robot = nh.advertise<iris_msgs::data_robots>("/communication/icast/data_robot", 1);
    sub_stm2pc = nh.subscribe<iris_msgs::stm2pc>("/hardware/stm2pc", 1, callbackSubscribeStm2pc);
    sub_final_pose = nh.subscribe<geometry_msgs::Pose2D>("/world_model/robot/pose", 1, callbackSubscribeFinalPose);
    sub_velocity = nh.subscribe<geometry_msgs::Pose2D>("/world_model/robot/vel", 1, callbackSubscribeVelocity);
    sub_robot_condition = nh.subscribe<std_msgs::Int16>("/master/robot/robot_condition", 1, callbackSubscribeRobotCondition);
    sub_target_intercept = nh.subscribe<geometry_msgs::Pose2D>("/master/ball/intercept", 1, callbackSubscribeTargetIntercept);
    sub_my_obs = nh.subscribe<sensor_msgs::PointCloud>("/world_model/obs/pos", 1, callbackSubscribeMyObs);
    // sub_my_role = nh.subscribe<std_msgs::UInt8>("/master/robot/role", 1, callbackSubscribeMyRole);
    tim_routine = nh.createTimer(ros::Duration(0.0001), callbackRoutine);
    // tim_send = nh.createTimer(ros::Duration(0.0001), callbackSend);
    // tim_recv = nh.createTimer(ros::Duration(0.0001), callbackRecv);

    sub_final_ball = nh.subscribe<geometry_msgs::Pose2D>("/world_model/ball/final_pos", 1, callbackSubscribeFinalBall);
    sub_ball_status_vision = nh.subscribe<std_msgs::UInt8>("/world_model/ball/status", 1, callbackSubscribeBallStatusVision);
    sub_ball_status_hardware = nh.subscribe<std_msgs::UInt8>("/hardware/ballhandling/status", 1, callbackSubscribeBallStatusHardware);
    sub_my_obs = nh.subscribe<sensor_msgs::PointCloud>("/world_model/obs/pos", 1, callbackSubscribeMyObs);
    sub_positioning = nh.subscribe<std_msgs::Float32MultiArray>("/world_model/positioning/titik_final_positioning", 1, callbackSubscribePositioning);

    send_to_bs = 1;

    spinner.spin();
    return 0;
}

void callbackRoutine(const ros::TimerEvent& event)
{
    if (send_to_bs == 1) {
        icast->dc->setDataToBeSent("epoch", (void*)&epoch);
        icast->update();
        epoch.data++;
    }

    // if (icast->dc->isUpdateRemote())
    // {
    //     transmitAll();
    // }
    // else
    // {
    // }
    static uint16_t counter = 0;
    // printf("counter : %d\n", counter);
    if (counter++ > 5) {
        transmitAll();
        counter = 0;
    }
}

void callbackSend(const ros::TimerEvent& event)
{
    if (send_to_bs == 1) {
        icast->dc->setDataToBeSent("epoch", (void*)&epoch);
        icast->update_send();
        epoch.data++;
    }
}

void callbackRecv(const ros::TimerEvent& event)
{
    icast->update_receive();

    static uint16_t counter = 0;
    if (counter++ > 200) {
        transmitAll();
        counter = 0;
    }
}

void callbackSubscribeFinalPose(const geometry_msgs::Pose2DConstPtr& msg)
{
    pos_t pose;
    pose.x = msg->x;
    pose.y = msg->y;
    pose.theta = msg->theta;

    icast->dc->setDataToBeSent("pos", (void*)&pose);

    // icast->dc->setDataToBeSent("epoch", (void *)&epoch);
    // icast->update();
    // epoch.data++;
}

void callbackSubscribeMyObs(const sensor_msgs::PointCloudConstPtr& msg)
{
    obstacle_t obs;

    memset(&obs, 0, sizeof(obstacle_t));

    for (size_t i = 0; i < msg->points.size(); i++) {
        if (i >= 10)
            break;

        obs.pcl_x[i] = msg->points[i].x;
        obs.pcl_y[i] = msg->points[i].y;
    }

    icast->dc->setDataToBeSent("obstacle", (void*)&obs);
}

void callbackSubscribePositioning(const std_msgs::Float32MultiArrayConstPtr& msg)
{
    target_positioning_t target_positioning;
    target_positioning.x = msg->data.at(0);
    target_positioning.y = msg->data.at(1);
    icast->dc->setDataToBeSent("target_positioning", (void*)&target_positioning);
}

void callbackSubscribeStm2pc(const iris_msgs::stm2pcConstPtr& msg)
{
    battery_t battery;
    battery.voltage = msg->battery;
    icast->dc->setDataToBeSent("battery", (void*)&battery);

    send_to_bs = !((msg->buttons & 0b01000000) >> 0x06);
}

void callbackSubscribeVelocity(const geometry_msgs::Pose2DConstPtr& msg)
{
    vel_t vel;
    vel.x = msg->x;
    vel.y = msg->y;
    vel.theta = msg->theta;
    icast->dc->setDataToBeSent("vel", (void*)&vel);
}

void callbackSubscribeRobotCondition(const std_msgs::Int16ConstPtr& msg)
{
    state_machine_t state;
    memset(&state, 0, sizeof(state_machine_t));

    state.robot_condition = msg->data;
    icast->dc->setDataToBeSent("state_machine", (void*)&state);
}

void callbackSubscribeFinalBall(const geometry_msgs::Pose2DConstPtr& msg)
{
    ball.x[0] = (int16_t)msg->x;
    ball.y[0] = (int16_t)msg->y;
    icast->dc->setDataToBeSent("ball", (void*)&ball);
}

void callbackSubscribeBallStatusVision(const std_msgs::UInt8ConstPtr& msg)
{
    ball.is_visible = msg->data;
    icast->dc->setDataToBeSent("ball", (void*)&ball);
}

void callbackSubscribeBallStatusHardware(const std_msgs::UInt8ConstPtr& msg)
{
    ball.is_caught = msg->data;
    icast->dc->setDataToBeSent("ball", (void*)&ball);
}

void callbackSubscribeTargetIntercept(const geometry_msgs::Pose2DConstPtr& msg)
{
    prediction_t intercept;
    intercept.ball_x = msg->x;
    intercept.ball_y = msg->y;
    icast->dc->setDataToBeSent("prediction", (void*)&intercept);
}

void callbackSubscribeMyRole(const std_msgs::UInt8ConstPtr& msg)
{
    // role_t role;
    // role.data = msg->data;
    // icast->dc->setDataToBeSent("role", (void *)&role);
}

void transmitAll()
{
    /**
     * * ====================================================
     * * Basestation Data
     * * ====================================================
     */
    iris_msgs::bs2pc msg_bs2pc;
    msg_bs2pc.mode = icast->dc->data_bus.agent0.mode_base.data;
    msg_bs2pc.command = icast->dc->data_bus.agent0.command.data;
    msg_bs2pc.style = icast->dc->data_bus.agent0.style.data;
    msg_bs2pc.offset_robot_theta = icast->dc->data_bus.agent0.offset_robot.theta;
    msg_bs2pc.offset_robot_x = icast->dc->data_bus.agent0.offset_robot.x;
    msg_bs2pc.offset_robot_y = icast->dc->data_bus.agent0.offset_robot.y;
    msg_bs2pc.target_manual_theta = icast->dc->data_bus.agent0.target_manual.theta;
    msg_bs2pc.target_manual_x = icast->dc->data_bus.agent0.target_manual.x;
    msg_bs2pc.target_manual_y = icast->dc->data_bus.agent0.target_manual.y;
    msg_bs2pc.data_mux_1 = icast->dc->data_bus.agent0.data_mux.mux_1;
    msg_bs2pc.data_mux_2 = icast->dc->data_bus.agent0.data_mux.mux_2;
    msg_bs2pc.data_mux_control = icast->dc->data_bus.agent0.data_mux.mux_control;
    msg_bs2pc.pass_counter = icast->dc->data_bus.agent0.pass_counter.data;
    msg_bs2pc.index_obs_keeper = icast->dc->data_bus.agent0.index_obs.data[0];
    msg_bs2pc.index_obs_center = icast->dc->data_bus.agent0.index_obs.data[1];
    msg_bs2pc.index_obs_side = icast->dc->data_bus.agent0.index_obs.data[2];

    // for (int i = 0; i < 3; i++)
    // {
    //     printf("%d ", icast->dc->data_bus.agent0.index_obs.data[i]);
    // }
    // printf("\n");
    // ROS_ERROR("%d", msg_bs2pc.data_mux_control);
    // Scaler for the robot number from basestation
    static int8_t num_robot = atoi(getenv("ROBOT"));
    if (num_robot > 0) {
        msg_bs2pc.trim_translation_vel = icast->dc->data_bus.agent0.trim.translation_vel[num_robot - 1];
        msg_bs2pc.trim_rotation_vel = icast->dc->data_bus.agent0.trim.rotation_vel[num_robot - 1];
        msg_bs2pc.trim_kick_power = icast->dc->data_bus.agent0.trim.kick_power[num_robot - 1];
    }

    pub_bs2pc.publish(msg_bs2pc);

    /**
     * * ====================================================
     * * Robots Data
     * * ====================================================
     */
    static uint8_t epoch_robot[4] = { 0 };
    static uint8_t prev_epoch_robot[4] = { 0 };
    static uint16_t counter_untuk_reset[4] = { 0 };
    iris_msgs::data_robots msg_data_robot;

    msg_data_robot.pose_x.reserve(4);
    msg_data_robot.pose_y.reserve(4);
    msg_data_robot.pose_theta.reserve(4);

    msg_data_robot.ball_is_caught.reserve(4);
    msg_data_robot.ball_is_visible.reserve(4);

    msg_data_robot.robot_condition.reserve(4);
    msg_data_robot.fsm_0.reserve(4);
    msg_data_robot.fsm_1.reserve(4);
    msg_data_robot.fsm_2.reserve(4);

    msg_data_robot.target_positioning_x.reserve(4);
    msg_data_robot.target_positioning_y.reserve(4);

    msg_data_robot.pose_x.resize(4);
    msg_data_robot.pose_y.resize(4);
    msg_data_robot.pose_theta.resize(4);

    msg_data_robot.ball_is_caught.resize(4);
    msg_data_robot.ball_is_visible.resize(4);

    msg_data_robot.robot_condition.resize(4);
    msg_data_robot.fsm_0.resize(4);
    msg_data_robot.fsm_1.resize(4);
    msg_data_robot.fsm_2.resize(4);

    msg_data_robot.target_positioning_x.resize(4);
    msg_data_robot.target_positioning_y.resize(4);

    static agent1_t* agent_ptr[4];

    agent_ptr[1] = &icast->dc->data_bus.agent1;
    agent_ptr[2] = (agent1_t*)&icast->dc->data_bus.agent2;
    agent_ptr[3] = (agent1_t*)&icast->dc->data_bus.agent3;

    // UNTUK RESET DATA KETIKA ROBOT MATI
    for (uint8_t i = 1; i < 4; i++) {
        if (prev_epoch_robot[i] == epoch_robot[i] && epoch_robot[i] > 0 && prev_epoch_robot[i] > 0) {
            counter_untuk_reset[i]++;
        } else {
            counter_untuk_reset[i] = 0;
        }

        if (counter_untuk_reset[i] >= 150) {
            size_t offset_to_reset;
            size_t size_to_reset;
            icast->dc->getOffsetSize(i, "", offset_to_reset, size_to_reset);
            memset(&icast->dc->dictionary_data_[0] + offset_to_reset, 0, size_to_reset);
            memset(agent_ptr[i], 0, sizeof(agent1_t));
        }
    }

    for (uint8_t i = 1; i < 4; i++) {
        memcpy(&msg_data_robot.pose_x[i], &agent_ptr[i]->pos.x, sizeof(int16_t));
        memcpy(&msg_data_robot.pose_y[i], &agent_ptr[i]->pos.y, sizeof(int16_t));
        memcpy(&msg_data_robot.pose_theta[i], &agent_ptr[i]->pos.theta, sizeof(float));

        memcpy(&msg_data_robot.ball_is_caught[i], &agent_ptr[i]->ball.is_caught, sizeof(uint8_t));
        memcpy(&msg_data_robot.ball_is_visible[i], &agent_ptr[i]->ball.is_visible, sizeof(uint8_t));

        memcpy(&msg_data_robot.robot_condition[i], &agent_ptr[i]->state_machine.robot_condition, sizeof(int16_t));
        memcpy(&msg_data_robot.fsm_0[i], &agent_ptr[i]->state_machine.fsm_0, sizeof(int16_t));
        memcpy(&msg_data_robot.fsm_1[i], &agent_ptr[i]->state_machine.fsm_1, sizeof(int16_t));
        memcpy(&msg_data_robot.fsm_2[i], &agent_ptr[i]->state_machine.fsm_2, sizeof(int16_t));
        memcpy(&prev_epoch_robot[i], &epoch_robot[i], sizeof(uint8_t));
        memcpy(&epoch_robot[i], &agent_ptr[i]->epoch, sizeof(uint8_t));

        memcpy(&msg_data_robot.target_positioning_x[i], &agent_ptr[i]->target_positioning.x, sizeof(int16_t));
        memcpy(&msg_data_robot.target_positioning_y[i], &agent_ptr[i]->target_positioning.y, sizeof(int16_t));
    }

    // for (uint8_t i = 0; i < sizeof(&agent_ptr[1]->ball.x); i++)
    // {
    //     if (agent_ptr[1]->ball.x[i] != 0 && agent_ptr[1]->ball.y[i] != 0)
    //     {
    //         msg_data_robot.ball_pos_x_1.push_back(agent_ptr[1]->ball.x[i]);
    //         msg_data_robot.ball_pos_y_1.push_back(agent_ptr[1]->ball.y[i]);
    //     }
    // }

    for (uint8_t i = 0; i < sizeof(&agent_ptr[2]->ball.x); i++) {
        if (agent_ptr[2]->ball.x[i] != 0 && agent_ptr[2]->ball.y[i] != 0) {
            msg_data_robot.ball_pos_x_2.push_back(agent_ptr[2]->ball.x[i]);
            msg_data_robot.ball_pos_y_2.push_back(agent_ptr[2]->ball.y[i]);
        }
    }

    for (uint8_t i = 0; i < sizeof(&agent_ptr[3]->ball.x); i++) {
        if (agent_ptr[3]->ball.x[i] != 0 && agent_ptr[3]->ball.y[i] != 0) {
            msg_data_robot.ball_pos_x_3.push_back(agent_ptr[3]->ball.x[i]);
            msg_data_robot.ball_pos_y_3.push_back(agent_ptr[3]->ball.y[i]);
        }
    }

    // for (uint16_t i = 0; i < sizeof(&agent_ptr[1]->obstacle.pcl_x); i++)
    // {
    //     if (agent_ptr[1]->obstacle.pcl_x[i] != 0 && agent_ptr[1]->obstacle.pcl_y[i] != 0)
    //     {
    //         msg_data_robot.obs_pcl_x_1.push_back(agent_ptr[1]->obstacle.pcl_x[i]);
    //         msg_data_robot.obs_pcl_y_1.push_back(agent_ptr[1]->obstacle.pcl_y[i]);
    //     }
    // }

    for (uint16_t i = 0; i < sizeof(&agent_ptr[2]->obstacle.pcl_x); i++) {
        if (agent_ptr[2]->obstacle.pcl_x[i] != 0 && agent_ptr[2]->obstacle.pcl_y[i] != 0) {
            msg_data_robot.obs_pcl_x_2.push_back(agent_ptr[2]->obstacle.pcl_x[i]);
            msg_data_robot.obs_pcl_y_2.push_back(agent_ptr[2]->obstacle.pcl_y[i]);
        }
    }

    for (uint16_t i = 0; i < sizeof(&agent_ptr[3]->obstacle.pcl_x); i++) {
        if (agent_ptr[3]->obstacle.pcl_x[i] != 0 && agent_ptr[3]->obstacle.pcl_y[i] != 0) {
            msg_data_robot.obs_pcl_x_3.push_back(agent_ptr[3]->obstacle.pcl_x[i]);
            msg_data_robot.obs_pcl_y_3.push_back(agent_ptr[3]->obstacle.pcl_y[i]);
        }
    }

    pub_data_robot.publish(msg_data_robot);
}
