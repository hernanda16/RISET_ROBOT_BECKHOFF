#include "hardware/stm_udp_2023.h"

int main(int argc, char** argv)
{
    ros::init(argc, argv, "comm_stm");
    ros::NodeHandle NH;
    ros::MultiThreadedSpinner spinner(0);

    // Create UDP socket
    if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("cannot create socket\n");
        return 0;
    }

    // Set PC conf
    bzero(&myaddr, sizeof(struct sockaddr_in));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(RECEIVE_PORT);
    myaddr.sin_addr.s_addr = INADDR_ANY;

    // Set STM addr conf
    bzero(&dst_addr, sizeof(struct sockaddr_in));
    dst_addr.sin_family = AF_INET;
    dst_addr.sin_port = htons(SEND_PORT);
    dst_addr.sin_addr.s_addr = inet_addr("169.254.183.100");

    // Bind for recv data from STM
    if (bind(fd, (struct sockaddr*)&myaddr, sizeof(myaddr)) < 0) {
        perror("bind failed");
        return 0;
    }

    pub_stm2pc = NH.advertise<iris_msgs::stm2pc>("/hardware/stm2pc", 0);

    recv_timer = NH.createTimer(ros::Duration(0.0001), CllbckRecv);
    send_timer = NH.createTimer(ros::Duration(0.01), CllbckSend);

    sub_pc2stm = NH.subscribe("/master/pc2stm", 1, CllbckPc2Stm);
    sub_dribble_vel = NH.subscribe("/hardware/ballhandling/stm", 1, CllbckVelDribble);

    // sub_vel_data = NH.subscribe("cmd_vel_motor", 1, CllbckVelMotor);
    // sub_pc2stm = NH.subscribe("pc2stm", 1, CllbckPc2Stm);
    // sub_dribble_vel = NH.subscribe("dribble2stm", 1, CllbckVelDribble);
    // sub_master_offset = NH.subscribe("master_offset", 1, CllbckMasterOffset);
    // sub_realtime_position = NH.subscribe("robot_position", 1, CllbckRealtimePositition);

#ifdef GOALKEEPER
    //---Pub
    //======
    pub_keeper_data = NH.advertise<iris_msgs::keeper_data>("/srf_data", 1);
    //---Sub
    //======
    sub_extend = NH.subscribe("extend_data", 1, CllbckExtend);
#endif

    spinner.spin();

    recv_timer.stop();
    send_timer.stop();

    return 0;
}
void CllbckPc2Stm(const iris_msgs::pc2stmConstPtr& msg)
{
    // odom_offset[0] = msg->odom_offset_x;
    // odom_offset[1] = msg->odom_offset_y;
    // odom_offset[2] = msg->odom_offset_th;
    buzzer_cnt = msg->buzzer_cnt;
    buzzer_time = msg->buzzer_time;
    force_dribble_control = msg->force_dribble_control;
}

void CllbckRecv(const ros::TimerEvent& event)
{
    static double prev_time = 0;

    static uint8_t last_gp[2], gp_buffer[2];
    static uint8_t last_throttle[2], throttle_buffer[2];

    // UDP recv
    int8_t recvlen = recvfrom(fd, recv_buffer, 64, MSG_DONTWAIT, (struct sockaddr*)&stmaddr, &addr_len);
    if (recvlen > 0 && recv_buffer[0] == 'i' && recv_buffer[1] == 't' && recv_buffer[2] == 's') {
        // ROS_ERROR("stm hz %.4f", ros::Time::now().toSec() - prev_time);
        iris_msgs::stm2pc msg;
        // memcpy(&msg.odom_buffer_x, recv_buffer + 3, 4);
        // memcpy(&msg.odom_buffer_y, recv_buffer + 7, 4);
        memcpy(&msg.odom_buffer_th, recv_buffer + 11, 4);
        msg.odom_buffer_th = -msg.odom_buffer_th;
        // msg.odom_buffer_th = -msg.odom_buffer_th;
        // memcpy(&msg.line_sensors, recv_buffer + 15, 1);
#ifndef GOALKEEPER
        memcpy(&gp_buffer[0], recv_buffer + 16, 1);
        memcpy(&gp_buffer[1], recv_buffer + 17, 1);
        memcpy(&msg.buttons, recv_buffer + 18, 1);
        // memcpy(&msg.ethernet_communication, recv_buffer + 19, 4);
        // memcpy(&msg.left_enc, recv_buffer + 23, 2);
        // memcpy(&msg.right_enc, recv_buffer + 25, 2);
        memcpy(&msg.right_dribble_enc_vx, recv_buffer + 27, 2);
        memcpy(&msg.left_dribble_enc_vx, recv_buffer + 29, 2);
        memcpy(&throttle_buffer[0], recv_buffer + 31, 1);
        memcpy(&throttle_buffer[1], recv_buffer + 32, 1);
        memcpy(&msg.right_dribble_enc_vx, recv_buffer + 33, 2);
        memcpy(&msg.left_dribble_enc_vx, recv_buffer + 35, 2);
        // memcpy(&msg.vth, recv_buffer + 37, 2);
        memcpy(&msg.current_kicker_position, recv_buffer + 39, 2);
        memcpy(&msg.battery, recv_buffer + 41, 4);
        memcpy(&msg.left_enc_px, recv_buffer + 45, 2);
        memcpy(&msg.right_enc_px, recv_buffer + 47, 2);
        // msg.gyro_buffer = msg.odom_buffer_th;
#else
        iris_msgs::keeper_data keeper_msgs;
        memcpy(&msg.buttons, recv_buffer + 18, 1);
        memcpy(&msg.left_enc, recv_buffer + 19, 2);
        memcpy(&msg.right_enc, recv_buffer + 21, 2);
        keeper_msgs.srf.push_back(recv_buffer[23]);
        keeper_msgs.srf.push_back(recv_buffer[24]);
        keeper_msgs.srf.push_back(recv_buffer[25]);
        pub_keeper_data.publish(keeper_msgs);
#endif

        msg.gp_sensors_left = gp_buffer[0];
        msg.gp_sensors_right = gp_buffer[1];
        msg.left_throttle = throttle_buffer[0];
        msg.right_throttle = throttle_buffer[1];

        last_gp[0] = msg.gp_sensors_left;
        last_gp[1] = msg.gp_sensors_right;
        last_throttle[0] = msg.left_throttle;
        last_throttle[1] = msg.right_throttle;

        // ROS_INFO("data vth: %d", msg.vth);
        pub_stm2pc.publish(msg);

        prev_time = ros::Time::now().toSec();
    }
}
void CllbckSend(const ros::TimerEvent& event)
{
    static uint8_t reset_buzzer = 0;
    //---Robot Velocity
    //=================
    memcpy(send_buffer + 3, &velocity_robot, 3);
    //---Robot Position
    //=================
    memcpy(send_buffer + 6, &realtime_position[0], 4);
    memcpy(send_buffer + 10, &realtime_position[1], 4);
    memcpy(send_buffer + 14, &odom_offset[2], 4);
    memcpy(send_buffer + 35, &perfect_backward_arm, 2);
    memcpy(send_buffer + 37, &force_dribble_control, 1);

#ifndef GOALKEEPER
    //---Kicker
    //=========
    // printf("kick %d %.2f\n", kicker_mode, kicker_power);
    memcpy(send_buffer + 18, &kicker_mode, 1);
    memcpy(send_buffer + 19, &kicker_power, 4);
    //---Buzzer
    //=========
    memcpy(send_buffer + 23, &buzzer_cnt, 1);
    memcpy(send_buffer + 24, &buzzer_time, 1);
    //---Dribble
    //==========
    // printf("DRIBLE VEL%d || %d\n",right_dribble_speed,left_dribble_speed);
    memcpy(send_buffer + 25, &right_dribble_speed, 2);
    memcpy(send_buffer + 27, &left_dribble_speed, 2);
    //---Kicker Position
    //==================
    // kicker_position = 166;
    memcpy(send_buffer + 29, &kicker_position, 2);
    memcpy(send_buffer + 33, &global_kicker_position, 2);

#else
    //---Buzzer
    //=========
    memcpy(send_buffer + 18, &buzzer_cnt, 1);
    memcpy(send_buffer + 19, &buzzer_time, 1);
    //---Extend
    //=========
    memcpy(send_buffer + 20, &right_extend_mode, 2);
    memcpy(send_buffer + 22, &right_extend_delay, 2);
    memcpy(send_buffer + 24, &left_extend_mode, 2);
    memcpy(send_buffer + 26, &left_extend_delay, 2);
#endif

    // for (uint8_t i = 0; i < 100; i++)
    sendto(fd, (void*)send_buffer, 38, 0, (struct sockaddr*)&dst_addr, dst_len);
}

void CllbckVelMotor(const geometry_msgs::TwistConstPtr& msg)
{
    velocity_robot[0] = (int)(msg->linear.x);
    velocity_robot[1] = (int)(msg->linear.y);
    velocity_robot[2] = (int)(msg->angular.z);
}
void CllbckVelDribble(const std_msgs::Int16MultiArrayConstPtr& msg)
{
    right_dribble_speed = msg->data.at(0);
    left_dribble_speed = msg->data.at(1);
    kicker_power = msg->data.at(2) * 0.01;
    global_kicker_position = msg->data.at(3);
    perfect_backward_arm[0] = msg->data.at(5);
    perfect_backward_arm[1] = msg->data.at(4);
    kicker_mode = msg->data.at(6);
    kicker_position = msg->data.at(3); // Unused
}

void CllbckMasterOffset(const geometry_msgs::Pose2DConstPtr& msg)
{
    odom_offset[0] = msg->x;
    odom_offset[1] = msg->y;
    odom_offset[2] = msg->theta;
}

void CllbckRealtimePositition(const geometry_msgs::Pose2DConstPtr& msg)
{
    realtime_position[0] = msg->x;
    realtime_position[1] = msg->y;
    realtime_position[2] = msg->theta;
}

#ifdef GOALKEEPER
void CllbckExtend(const std_msgs::Int16MultiArrayConstPtr& msg)
{

    right_extend_mode = msg->data.at(0);
    right_extend_delay = msg->data.at(1);
    left_extend_mode = msg->data.at(2);
    left_extend_delay = msg->data.at(3);
}
#endif