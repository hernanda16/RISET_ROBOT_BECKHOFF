/**
 * @author Hernanda Achmad Priyatna - hernanda16
 * @brief Handling communication with motor driver by rs232
 * * Datasheets:
 * ? https://www.pk-rus.ru/fileadmin/download/simpleiq_command_referense_manual.pdf
 * ? https://www.motorpowerco.com/media/filer_public/e6/6b/e66b3d73-815d-465d-af5a-f4379ae9d8c4/man-solgui.pdf
 */

#include "hardware/motor_rs232.hpp"

void Motor_RS232::init(ros::NodeHandle* nh)
{
    this->loadConfig();

    this->sub_stm2pc = nh->subscribe("/hardware/stm2pc", 1, &Motor_RS232::callbackSubcribeSTM2PC, this);
    this->sub_velocity = nh->subscribe("/master/robot/vel", 1, &Motor_RS232::callbackSubcribeVel, this);
    this->sub_additional_vel_ballhandling = nh->subscribe("/hardware/ballhandling/motor", 1, &Motor_RS232::callbackSubcribeAdditionalVelBallhandling, this);

    this->pub_enc_motor = nh->advertise<std_msgs::Int32MultiArray>("/hardware/motor/enc", 1);

    this->tim_routine = nh->createTimer(ros::Duration(0.02), std::bind(&Motor_RS232::callbackRoutine, this, std::placeholders::_1));
}

void Motor_RS232::loadConfig()
{
    char* robot_num = getenv("ROBOT");
    char config_file[100];

    std::string current_dir = ros::package::getPath("hardware");
    sprintf(config_file, "%s/../../config/IRIS%s/static_conf.yaml", current_dir.c_str(), robot_num);

    YAML::Node config = YAML::LoadFile(config_file);
    left_motor = config["Motor"]["left_motor_port"].as<int>();
    right_motor = config["Motor"]["right_motor_port"].as<int>();
    rear_motor = config["Motor"]["rear_motor_port"].as<int>();
    k_pi_left[0] = config["Motor"]["kp_left_motor"].as<int>();
    k_pi_left[1] = config["Motor"]["ki_left_motor"].as<int>();
    k_pi_right[0] = config["Motor"]["kp_right_motor"].as<int>();
    k_pi_right[1] = config["Motor"]["ki_right_motor"].as<int>();
    k_pi_rear[0] = config["Motor"]["kp_rear_motor"].as<int>();
    k_pi_rear[1] = config["Motor"]["ki_rear_motor"].as<int>();

    printf("PORT MOTOR: %d %d %d\n", left_motor, right_motor, rear_motor);

    uint8_t flag_motor = 0b00000000;

    //=================================================================================================
    if (RS232_OpenComport(left_motor, BAUD_RATE, mode)) {
        SET_BIT(flag_motor, 1);
    }
    if (RS232_OpenComport(right_motor, BAUD_RATE, mode)) {
        SET_BIT(flag_motor, 2);
    }
    if (RS232_OpenComport(rear_motor, BAUD_RATE, mode)) {
        SET_BIT(flag_motor, 3);
    }

    if (flag_motor != 0b00000000) {
        if (CHECK_BIT(flag_motor, 1))
            ROS_ERROR("MOTOR RS232 ERROR : Left Motor Died");

        if (CHECK_BIT(flag_motor, 2))
            ROS_ERROR("MOTOR RS232 ERROR : Right Motor Died");

        if (CHECK_BIT(flag_motor, 3))
            ROS_ERROR("MOTOR RS232 ERROR : Rear Motor Died");

        ros::shutdown();
    }
}

void Motor_RS232::callbackRoutine(const ros::TimerEvent& event)
{
    static struct prev_state_tag {
        uint8_t left_motor = 0;
        uint8_t right_motor = 0;
        uint8_t rear_motor = 0;
    } prev_state;

    struct current_state_tag {
        uint8_t left_motor = 0;
        uint8_t right_motor = 0;
        uint8_t rear_motor = 0;
    } current_state;

    unsigned char recv_buffer_left_motor[512];
    unsigned char recv_buffer_right_motor[512];
    unsigned char recv_buffer_rear_motor[512];

    char send_buffer_left_motor[128];
    char send_buffer_right_motor[128];
    char send_buffer_rear_motor[128];

    current_state.left_motor = RS232_PollComport(left_motor, recv_buffer_left_motor, sizeof(recv_buffer_left_motor));
    current_state.right_motor = RS232_PollComport(right_motor, recv_buffer_right_motor, sizeof(recv_buffer_right_motor));
    current_state.rear_motor = RS232_PollComport(rear_motor, recv_buffer_rear_motor, sizeof(recv_buffer_rear_motor));

    RS232_flushRX(left_motor);
    RS232_flushRX(right_motor);
    RS232_flushRX(rear_motor);

    const char control_cmd[64] = "IL[3]=3;UM=2;MO=1;";

    if (prev_state.left_motor == 0x00 && current_state.left_motor > 0b00) {
        printf("LEFT MOTOR: %d\n", current_state.left_motor);
        RS232_cputs(left_motor, control_cmd);
    }
    if (prev_state.right_motor == 0x00 && current_state.right_motor > 0b00) {
        printf("RIGHT MOTOR: %d\n", current_state.right_motor);
        RS232_cputs(right_motor, control_cmd);
    }
    if (prev_state.rear_motor == 0x00 && current_state.rear_motor > 0b00) {
        printf("REAR MOTOR: %d\n", current_state.rear_motor);
        RS232_cputs(rear_motor, control_cmd);
    }

    static encoder_t rear_motor_enc;
    static encoder_t left_motor_enc;
    static encoder_t right_motor_enc;

    rear_motor_enc.angle = 0;
    left_motor_enc.angle = 240;
    right_motor_enc.angle = 120;

    GetMotorEnc(recv_buffer_left_motor, current_state.left_motor, &left_motor_enc);
    GetMotorEnc(recv_buffer_right_motor, current_state.right_motor, &right_motor_enc);
    GetMotorEnc(recv_buffer_rear_motor, current_state.rear_motor, &rear_motor_enc);

    left_motor_enc.x_robot = left_motor_enc.base_enc * cosf(left_motor_enc.angle DEG2RAD);
    left_motor_enc.y_robot = left_motor_enc.base_enc * sinf(left_motor_enc.angle DEG2RAD);
    right_motor_enc.x_robot = right_motor_enc.base_enc * cosf(right_motor_enc.angle DEG2RAD);
    right_motor_enc.y_robot = right_motor_enc.base_enc * sinf(right_motor_enc.angle DEG2RAD);
    rear_motor_enc.x_robot = rear_motor_enc.base_enc * cosf(rear_motor_enc.angle DEG2RAD);
    rear_motor_enc.y_robot = rear_motor_enc.base_enc * sinf(rear_motor_enc.angle DEG2RAD);

    float vx = (rear_motor_enc.x_robot + left_motor_enc.x_robot + right_motor_enc.x_robot) * ENC2CM;
    float vy = (rear_motor_enc.y_robot + left_motor_enc.y_robot + right_motor_enc.y_robot) * ENC2CM;
    float vth = (rear_motor_enc.base_enc + left_motor_enc.base_enc + right_motor_enc.base_enc) * ENC2DEG;

    static float pos_buffer[3] = { 0 };
    static float pos_global[3] = { 0, 0, 90 };

    pos_buffer[0] += vx;
    pos_buffer[1] += vy;
    pos_buffer[2] += vth;

    while (pos_buffer[2] > 180)
        pos_buffer[2] -= 360;
    while (pos_buffer[2] < -180)
        pos_buffer[2] += 360;

    float vx_global = vx * sinf(pos_global[2] DEG2RAD) + vy * cosf(pos_global[2] DEG2RAD);
    float vy_global = vx * -cosf(pos_global[2] DEG2RAD) + vy * sinf(pos_global[2] DEG2RAD);
    float vth_global = vth;

    pos_global[0] += vx_global;
    pos_global[1] += vy_global;
    pos_global[2] += vth_global;

    while (pos_global[2] > 180)
        pos_global[2] -= 360;
    while (pos_global[2] < -180)
        pos_global[2] += 360;

    // printf("v: %.2f %.2f %.2f || %d %d %d || %.2f %.2f %.2f || %.2f %.2f %.2f\n",
    //        vx, vy, vth, left_motor_enc.base_enc, right_motor_enc.base_enc, rear_motor_enc.base_enc,
    //        pos_buffer[0], pos_buffer[1], pos_buffer[2],
    //        pos_global[0], pos_global[1], pos_global[2]);

    std_msgs::Int32MultiArray msg_enc_motor;
    msg_enc_motor.data.push_back(left_motor_enc.base_enc);
    msg_enc_motor.data.push_back(right_motor_enc.base_enc);
    msg_enc_motor.data.push_back(rear_motor_enc.base_enc);
    pub_enc_motor.publish(msg_enc_motor);

    static float vel_left_motor;
    static float vel_right_motor;
    static float vel_rear_motor;

    /* Addition vel to handle the ball, if ball is left from robot, so robot will turn left. Based on throttle*/
    float error_throttle = add_vel[0] - add_vel[1];
    float add_vth = error_throttle / 5;
    float add_vforward = fabs(error_throttle) / 2.5; // / 2.5
    // add_vforward = fabs(error_throttle) / -3;

    // Convert angular vel_th to linear for each motors. L = a . r
    float used_vth = robot_vel.theta DEG2RAD * MOTOR2CENTER + add_vth;
    float used_vy = robot_vel.y + add_vforward;

    vel_left_motor = used_vth + robot_vel.x * cosf(left_motor_enc.angle DEG2RAD) + used_vy * sinf(left_motor_enc.angle DEG2RAD);
    vel_right_motor = used_vth + robot_vel.x * cosf(right_motor_enc.angle DEG2RAD) + used_vy * sinf(right_motor_enc.angle DEG2RAD);
    vel_rear_motor = used_vth + robot_vel.x;

    vel_left_motor = vel_left_motor / MOTOR_RADIUS * RAD2CNTS * 1;
    vel_right_motor = vel_right_motor / MOTOR_RADIUS * RAD2CNTS * 1;
    vel_rear_motor = vel_rear_motor / MOTOR_RADIUS * RAD2CNTS * 1; // 6959

    sprintf(send_buffer_left_motor, "VX;JV=%d;BG;KP[2]=%d;KI[2]=%d;MO=%d;", (int)(vel_left_motor * CHECK_BIT(buttons, 7)), k_pi_left[0], k_pi_left[1], CHECK_BIT(buttons, 7));
    sprintf(send_buffer_right_motor, "VX;JV=%d;BG;KP[2]=%d;KI[2]=%d;MO=%d;", (int)(vel_right_motor * CHECK_BIT(buttons, 7)), k_pi_right[0], k_pi_right[1], CHECK_BIT(buttons, 7));
    sprintf(send_buffer_rear_motor, "VX;JV=%d;BG;KP[2]=%d;KI[2]=%d;MO=%d;", (int)(vel_rear_motor * CHECK_BIT(buttons, 7)), k_pi_rear[0], k_pi_rear[1], CHECK_BIT(buttons, 7));

    RS232_cputs(left_motor, send_buffer_left_motor);
    RS232_cputs(right_motor, send_buffer_right_motor);
    RS232_cputs(rear_motor, send_buffer_rear_motor);

    RS232_flushTX(left_motor);
    RS232_flushTX(right_motor);
    RS232_flushTX(rear_motor);

    prev_state.left_motor = current_state.left_motor;
    prev_state.right_motor = current_state.right_motor;
    prev_state.rear_motor = current_state.rear_motor;
}

void Motor_RS232::callbackSubcribeSTM2PC(const iris_msgs::stm2pcConstPtr& msg)
{
    /* buttons (bit-selection) (active low) */
    buttons = ~msg->buttons;
}

void Motor_RS232::callbackSubcribeVel(const geometry_msgs::Pose2DConstPtr& msg)
{
    robot_vel.x = msg->x;
    robot_vel.y = msg->y;
    robot_vel.theta = msg->theta;

    // TODO : Remove this after debugging
    //  robot_vel.x = fmax(-100, fmin(100, robot_vel.x));
    //  robot_vel.y = fmax(-100, fmin(100, robot_vel.y));
    //  robot_vel.theta = fmax(-100, fmin(100, robot_vel.theta));
}

void Motor_RS232::callbackSubcribeAdditionalVelBallhandling(const std_msgs::Float32MultiArrayConstPtr& msg)
{
    add_vel[0] = msg->data[0];
    add_vel[1] = msg->data[1];

    // if (msg->data.size() == 2)
    // {
    // }
}

/**
 * @author IRIS-ITS
 * @brief Combine three encoders with IMU from MPU6050
 * @param raw data from driver
 * @param size data size from driver
 * @param enc_ encoder struct
 */
void Motor_RS232::GetMotorEnc(unsigned char* raw, uint8_t size, encoder_t* enc_)
{
    unsigned char enc_buff[32] = { '0' }; // 0 default value
    unsigned char px_buff[32] = { '0' }; // 0 default value
    uint8_t cnt_found = 0;

    /**
     * Extract encoder from driver
     * Encoder velocity data is "VX:<data>"
     * Encoder position data is "PX:<data>"
     *
     * */
    for (uint8_t i = 0; i < size; i++) {
        if (raw[i] == 0x56) // V
        {
            if (raw[i + 1] == 0x58) // X
            {
                uint8_t buff_ctr = 0;
                uint8_t expected_count = -1;

                for (uint8_t j = i + 3; j < size; j++) {
                    /* Filter string from wild char */
                    if ((raw[j] >= '0' && raw[j] <= '9') || raw[j] == '-') {
                        enc_buff[buff_ctr] = raw[j];
                        buff_ctr++;
                    }
                    expected_count++;

                    if (raw[j] == 0x3B) // ;
                    {
                        break;
                    }
                }
                /* Filter string from wild char */
                if (buff_ctr != expected_count) {
                    // ROS_ERROR("DATA MOTOR BODOH (VX) (%d %d) %s", expected_count, buff_ctr, enc_buff);
                    enc_->base_enc = enc_->base_enc; // lawas
                    // enc_->base_enc = 0; // baru
                } else {
                    // ROS_WARN("GET (%d %d) %s", expected_count, buff_ctr, enc_buff);
                    enc_buff[buff_ctr] = '\0'; // Null terminated string
                    enc_->base_enc = atoi((const char*)enc_buff);
                }
                cnt_found++;
            }
        }
        if (raw[i] == 0x50) // P
        {
            if (raw[i + 1] == 0x58) // X
            {
                uint8_t buff_ctr = 0;
                uint8_t expected_count = -1;
                for (uint8_t j = i + 3; j < size; j++) {
                    /* Filter string from wild char */
                    if ((raw[j] >= '0' && raw[j] <= '9') || raw[j] == '-') {
                        px_buff[buff_ctr] = raw[j];
                        buff_ctr++;
                    }
                    expected_count++;
                    if (raw[j] == 0x3B) // ;
                    {
                        break;
                    }
                }
                /* Filter string from wild char */
                if (buff_ctr != expected_count) {
                    // ROS_ERROR("DATA MOTOR BODOH (PX) (%d %d) %s", expected_count, buff_ctr, px_buff);
                    enc_->px = enc_->px;
                } else {
                    px_buff[buff_ctr] = '\0'; // Null terminated string
                    enc_->px = atoi((const char*)px_buff);
                }
                cnt_found++;
            }
        }

        if (cnt_found == 0x02)
            break;
    }

    return;
}
