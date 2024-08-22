#include "hardware/stm_serial.hpp"

int main(int argc, char** argv)
{
    ros::init(argc, argv, "stm_serial");
    ros::NodeHandle nh;
    ros::MultiThreadedSpinner spinner(1);

    if (stmOpenSerial() == -1)
        ros::shutdown();

    sub_motor_pwm = nh.subscribe("/hardware/ballhandling/dribble_aux_pwm", 3, callbackMotorPwm);
    pub_motor_enc = nh.advertise<std_msgs::Int16MultiArray>("/hardware/stm/serial/dribble_aux_enc", 1);

    spinner.spin();

    return 0;
};

int8_t stmOpenSerial()
{
    // Open the serial port. Change device path as needed (currently set to an standard FTDI USB-UART cable type device)
    // serial_port_fd = open("/dev/ttyUSB0", O_RDWR);
    serial_port_fd = open("/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_A800G9MQ-if00-port0", O_RDWR);

    if (serial_port_fd < 0) {
        printf("Error %i from opening usb device: %s\n", errno, strerror(errno));
        return -1;
    }

    // Create new termios struct, we call it 'tty' for convention
    struct termios tty;

    // Read in existing settings, and handle any error
    if (tcgetattr(serial_port_fd, &tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return -1;
    }

    tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size
    tty.c_cflag |= CS8; // 8 bits per byte (most common)
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
    // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

    tty.c_cc[VTIME] = 0; // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;

    // Set in/out baud rate to be 9600
    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);

    // status |= TIOCM_DTR; /* turn on DTR */
    // status |= TIOCM_RTS; /* turn on RTS */

    // Save tty settings, also checking for error
    if (tcsetattr(serial_port_fd, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return -1;
    }

    return 0;
}

void callbackMotorPwm(const std_msgs::Int16MultiArray::ConstPtr& msg)
{
    /**
     * @brief This function is to control two dribble arm high,
     * @note The pwm_motor[0] is the left motor and pwm_motor[1] is the right motor
    */
    int16_t pwm_motor[2] = { 0, 0 };

    pwm_motor[0] = msg->data[0];
    pwm_motor[1] = msg->data[1];

    // ROS_INFO("PWM %d %d", pwm_motor[0], pwm_motor[1]);

    unsigned char send_buffer[7] = "its";

    memcpy(send_buffer + 3, &pwm_motor[0], 2);
    memcpy(send_buffer + 5, &pwm_motor[1], 2);

    write(serial_port_fd, (void*)send_buffer, 7);
    // printf("serial write : %d\n", serial_port_fd);

    unsigned char recv_buffer[14];
    int8_t recv_len = read(serial_port_fd, &recv_buffer, 7);
    // printf("serial read : %d\n", recv_len);
    // printf("serial read : %s\n", recv_buffer);
    memcpy(recv_buffer + 7, recv_buffer, 7); // Circular array rotation
    uint8_t iter = 0;
    uint8_t has_offset = 0;
    while (iter < recv_len + 7 && has_offset == 0) {
        if (recv_buffer[iter] == 'i' && recv_buffer[iter + 1] == 't' && recv_buffer[iter + 2] == 's') {
            int16_t left_enc_buffer;
            int16_t right_enc_buffer;

            memcpy(&left_enc_buffer, recv_buffer + 3 + iter, 2);
            memcpy(&right_enc_buffer, recv_buffer + 3 + iter + 2, 2);

            std_msgs::Int16MultiArray msg_enc;
            msg_enc.data.push_back(left_enc_buffer);
            msg_enc.data.push_back(right_enc_buffer);

            // printf("ENC %d %d ||\n", left_enc_buffer, right_enc_buffer);
            pub_motor_enc.publish(msg_enc);
            has_offset = 1;
        }
        iter++;
    }
}