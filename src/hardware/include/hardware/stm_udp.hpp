#ifndef STM_UDP_HPP
#define STM_UDP_HPP

//--ROS Headers
#include <ros/ros.h>

//--C++ Headers
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

//--ROS Messages
#include "iris_msgs/pc2stm.h"
#include "iris_msgs/stm2pc.h"
#include "std_msgs/Int16MultiArray.h"

#define PC_RECV_PORT 8888
#define STM_RECV_PORT 44444
#define RECV_UDP
#define SEND_UDP

class STM_UDP
{
private:
    STM_UDP(){};
    ~STM_UDP(){};

    //--UDP Connection
    /**
     * @note socket - return a file descriptor for the pc new socket
     * @note bind - give socket FD the local address ADDR (which is LEN bytes long)
     */
    int socket_;
    struct sockaddr_in server_;                  // STM Address (Need to configure to send)
    struct sockaddr_in any_addr_;                // Any Address sending to this PC
    socklen_t any_addr_len_ = sizeof(any_addr_); // Lenght of Any Address
    struct sockaddr_in client_;                  // PC Address (Need to configure to recv)

    //--File Descriptor
    fd_set readfds;

public:
    static STM_UDP *getInstance()
    {
        static STM_UDP instance;
        return &instance;
    }
    STM_UDP(STM_UDP const &) = delete;
    void operator=(STM_UDP const &) = delete;

    //--ROS Publisher
    ros::Publisher pub_stm2pc;

    //--ROS Subscriber
    ros::Subscriber sub_pc2stm;
    ros::Subscriber sub_ballhandling;

    //--ROS Timer
    ros::Timer tim_send;
    ros::Timer tim_recv;

#ifdef GOALKEEPER
    //--ROS Subscriber
    ros::Subscriber sub_extend;

    //--ROS Publisher
    ros::Publisher pub_keeper_data;
#endif

    //--Prototypes
    void init(ros::NodeHandle *nh);
    void initUDP();
    void sendUDP();
    void recvUDP();
    void callbackSend(const ros::TimerEvent &event);
    void callbackRecv(const ros::TimerEvent &event);
    void callbackSubscribePC2STM(const iris_msgs::pc2stmConstPtr &msg);
    void callbackSubscribeBallHandling(const std_msgs::Int16MultiArrayConstPtr &msg);

    //--Global Variables
    char recv_buffer[64];
    char send_buffer[64] = "its";
    struct stm_tag
    {
        /**
         * @param [0]: vx
         * @param [1]: vy
         * @param [2]: vth
         */
        int16_t velocity_robot[3];
        /**
         * @param [0]: x
         * @param [1]: y
         * @param [2]: th
         */
        // float odom_offset[3]; // TODO: REMOVE
        /**
         * @param [0]: x
         * @param [1]: y
         * @param [2]: th
         */
        // float realtime_position[3];//TODO: REMOVE
        uint8_t kicker_mode;
        float kicker_power;
        uint8_t buzzer_cnt;
        uint8_t buzzer_time;
        int16_t left_dribble_speed;
        int16_t right_dribble_speed;
        uint8_t perfect_backward_arm[2] = {69, 69};
        uint16_t kicker_position;
        uint8_t color_status;
        uint16_t global_kicker_position = 800;
#ifdef GOALKEEPER
        int right_extend_mode;
        int left_extend_mode;
        int right_extend_delay;
        int left_extend_delay;
#endif
    } stm_data;
};

#endif