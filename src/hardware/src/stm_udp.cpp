#include "hardware/stm_udp.hpp"

void STM_UDP::init(ros::NodeHandle *nh)
{
    initUDP();

    this->pub_stm2pc = nh->advertise<iris_msgs::stm2pc>("/hardware/stm2pc", 1);

    this->sub_pc2stm = nh->subscribe("/master/pc2stm", 1, &STM_UDP::callbackSubscribePC2STM, this);
    this->sub_ballhandling = nh->subscribe("/hardware/ballhandling/stm", 1, &STM_UDP::callbackSubscribeBallHandling, this);

    this->tim_send = nh->createTimer(ros::Duration(0.01), std::bind(&STM_UDP::callbackSend, this, std::placeholders::_1));
    this->tim_recv = nh->createTimer(ros::Duration(0.0001), std::bind(&STM_UDP::callbackRecv, this, std::placeholders::_1));
}

void STM_UDP::initUDP()
{
    socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

#ifdef SEND_UDP
    // Clean data and set STM configuration
    bzero(&server_, sizeof(server_));
    server_.sin_family = AF_INET;
    server_.sin_port = htons(STM_RECV_PORT);
    server_.sin_addr.s_addr = inet_addr("169.254.183.100");
#endif

#ifdef RECV_UDP
    // Clean data and set PC configuration
    bzero(&client_, sizeof(client_));
    client_.sin_family = AF_INET;
    client_.sin_port = htons(PC_RECV_PORT);
    client_.sin_addr.s_addr = INADDR_ANY;

    // set udp timeout
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 2000;
    setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);

    // Bind socket and checking is binding success
    bind(socket_, (struct sockaddr *)&client_, sizeof(client_));
    if (socket_ < 0)
    {
        ROS_ERROR("STM UDP ERROR : Binding Failed");
    }

#endif
}

void STM_UDP::sendUDP()
{
    static uint8_t reset_buzzer = 0;
    /*New Data from STM*/
    //=================================================================
    memcpy(send_buffer + 3, &stm_data.buzzer_cnt, 1);
    memcpy(send_buffer + 4, &stm_data.buzzer_time, 1);
    memcpy(send_buffer + 5, &stm_data.kicker_mode, 1);
    memcpy(send_buffer + 6, &stm_data.kicker_power, 4);
    if (stm_data.kicker_position != 0)
        memcpy(send_buffer + 10, &stm_data.kicker_position, 2);
    else
        memcpy(send_buffer + 10, &stm_data.global_kicker_position, 2);
    memcpy(send_buffer + 12, &stm_data.right_dribble_speed, 2);
    memcpy(send_buffer + 14, &stm_data.left_dribble_speed, 2);
    //=================================================================
    sendto(socket_, send_buffer, sizeof(send_buffer), 0, (struct sockaddr *)&server_, sizeof(server_));
}

void STM_UDP::recvUDP()
{
    int8_t recv_len = recvfrom(socket_, recv_buffer, 64, 0, (struct sockaddr *)&any_addr_, &any_addr_len_);
    // if (recv_len == -1)
    //  {
    //      printf("errno : %d, error : %s\n", errno, strerror(errno));
    //      return;
    //  }

    if (recv_len > 0 && recv_buffer[0] == 'i' && recv_buffer[1] == 't' && recv_buffer[2] == 's')
    {
        iris_msgs::stm2pc msg;
        // Hardware stuff
        memcpy(&msg.buttons, recv_buffer + 3, 1);
        memcpy(&msg.battery, recv_buffer + 4, 4);
        // Positioning stuff
        memcpy(&msg.odom_buffer_th, recv_buffer + 8, 4);
        memcpy(&msg.left_enc_px, recv_buffer + 12, 2);
        memcpy(&msg.right_enc_px, recv_buffer + 14, 2);
        // Ball Sensor
        memcpy(&msg.gp_sensors_left, recv_buffer + 16, 1);
        memcpy(&msg.gp_sensors_right, recv_buffer + 17, 1);
        // Dribble stuff
        memcpy(&msg.left_throttle, recv_buffer + 18, 1);
        memcpy(&msg.right_throttle, recv_buffer + 19, 1);
        memcpy(&msg.right_dribble_enc_vx, recv_buffer + 20, 2);
        memcpy(&msg.left_dribble_enc_vx, recv_buffer + 22, 2);
        memcpy(&msg.current_kicker_position, recv_buffer + 24, 2);

        // static float prev_left_throttle = msg.left_throttle;
        // static float prev_right_throttle = msg.right_throttle;

        // static float res_left_throttle = 0;
        // static float res_right_throttle = 0;

        // res_left_throttle = prev_left_throttle * 0.1 + (float)msg.left_throttle * 0.9;
        // res_right_throttle = prev_right_throttle * 0.1 + (float)msg.right_throttle * 0.9;
        // printf("Left Throttle : %f, Right Throttle : %f\n", res_left_throttle, res_right_throttle);

        // prev_left_throttle = res_left_throttle;
        // prev_right_throttle = res_right_throttle;

        // msg.left_throttle = (uint8_t)res_left_throttle;
        // msg.right_throttle = (uint8_t)res_right_throttle;

        // prev_left_throttle = msg.left_throttle;
        // prev_right_throttle = msg.right_throttle;

        this->pub_stm2pc.publish(msg);
    }
}

void STM_UDP::callbackSend(const ros::TimerEvent &event)
{
    this->sendUDP();
}

void STM_UDP::callbackRecv(const ros::TimerEvent &event)
{
    this->recvUDP();
}

void STM_UDP::callbackSubscribePC2STM(const iris_msgs::pc2stmConstPtr &msg)
{
    stm_data.buzzer_cnt = msg->buzzer_cnt;
    stm_data.buzzer_time = msg->buzzer_time;
    stm_data.global_kicker_position = msg->global_kicker_position;
}

void STM_UDP::callbackSubscribeBallHandling(const std_msgs::Int16MultiArrayConstPtr &msg)
{
    stm_data.right_dribble_speed = msg->data.at(0);
    stm_data.left_dribble_speed = msg->data.at(1);
    stm_data.kicker_power = msg->data.at(2) * 0.01;
    stm_data.kicker_position = msg->data.at(3);
    stm_data.perfect_backward_arm[0] = msg->data.at(5);
    stm_data.perfect_backward_arm[1] = msg->data.at(4);
    stm_data.kicker_mode = msg->data.at(6);
}
