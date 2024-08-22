/**
 * September, 2022
 *
 * Reference:
 * https://tldp.org/HOWTO/Multicast-HOWTO-6.html
 * https://man7.org/linux/man-pages/man2/recv.2.html
 * https://docs.oracle.com/cd/E19683-01/816-5042/sockets-137/index.html
 */

#include "geometry_msgs/Point32.h"

//--ROS Messages
#include "geometry_msgs/Pose2D.h"
#include "iris_msgs/bs2pc.h"
#include "iris_msgs/data_robots.h"
#include "iris_msgs/stm2pc.h"
#include "std_msgs/UInt8.h"

// #include "iris_msgs/Vision.h"
// #include "iris_msgs/mc_in.h"
// #include "iris_msgs/mc_out.h"
// #include "iris_msgs/stm2pc.h"
// #include <iris_msgs/mc_in_rbts.h>
#include <ros/package.h>
#include <ros/ros.h>

#include "yaml-cpp/yaml.h"
#include <arpa/inet.h>
#include <chrono>
#include <errno.h>
#include <fstream>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <signal.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "termios.h"
#include <future>
#include <sys/ioctl.h>

/*

0         10             20
0 2 4 6 8 10 12 14 16 18 20
!   @ #   !     @  #     !
*/
/*
int8_t kbhit()
{
    static const int STDIN = 0;
    static bool initialized = false;

    if (!initialized)
    {
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }

    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}
*/
/* Define pre-processor functions */
// #define PERR(txt, par...) \
    printf("ERROR: (%s / %s): " txt "\n", _FILE, __FUNCTION_, ##par)
// #define PERRNO(txt) \
    printf("ERROR: (%s / %s): " txt ": %s\n", _FILE, __FUNCTION_, strerror(errno))

#define RAD2DEG 57.295780
#define DEG2RAD 0.017452925
#define UDP_TRAVEL_TIME_MS 5                        // 2
#define UDP_TRAVEL_TIME_US UDP_TRAVEL_TIME_MS * 1E3 // Mikro
#define UDP_TRAVEL_TIME_S UDP_TRAVEL_TIME_MS * 1E-3 // Mikro
#define SYNCHRONOUS

// Socket
typedef struct multiSocket_tag
{
    struct sockaddr_in destAddress;
    int socketID;
} multiSocket_t;
typedef struct nw_config
{
    char multicast_ip[16];
    char iface[10];
    char identifier[1];
    unsigned int port;
    uint8_t compress_type;
} config;
multiSocket_t multiSocket;
multiSocket_t *recv_socket;

// Config
config nw_config;

ros::Timer recv_timer;
ros::SteadyTimer send_timer;

// ros::Publisher pub_bs2pc;
// ros::Publisher pub_sh_robot_data;
// ros::Subscriber sub_stm2pc;
// ros::Subscriber sub_mc_out;
// ros::Subscriber sub_obs_data;
// ros::Subscriber sub_icp_data;
// ros::Subscriber sub_passing_point; // A subscriber for shared robot data

//--Publisher
ros::Publisher pub_bs2pc;
ros::Publisher pub_data_robot;

//--Subscriber
ros::Subscriber sub_final_pose;
ros::Subscriber sub_stm2pc;
ros::Subscriber sub_ball_status;
ros::Subscriber sub_ball_pos;

// PC2MC_out datas
int16_t pos_x;
int16_t pos_y;
int16_t theta;
int8_t ball_status;
int16_t ball_x;
int16_t ball_y;
uint16_t robot_cond;
int8_t pass_target;
uint8_t target_on_field;
float battery = 0;
int16_t ball_x_next;
int16_t ball_y_next;
int16_t robot_x_next;
int16_t robot_y_next;
int8_t kick_mode;

// UDP data
char send_buf[512];
uint16_t actual_data_size;
char its[4] = "its";

/* Shared data robots */
uint8_t sh_data_valid[4];
int16_t sh_pos_x[4];
int16_t sh_pos_y[4];
int16_t sh_theta[4];
uint16_t sh_robot_cond[4];
uint8_t sh_total_obs[4];
uint8_t sh_target_on_field[4];
int16_t sh_target_pos_x[4];
int16_t sh_target_pos_y[4];
uint8_t sh_pass_target_num[4];
int16_t sh_obs_x_field[4][144];
int16_t sh_obs_y_field[4][144];

boost::mutex mutex_obs; // use mutex for obstacles

/* Robot's logic if it online or not */
uint8_t delay_tolerance = 5;
int64_t robot_epoch[6];

// Obs processing
uint16_t obs_on_field[145];
uint8_t total_obs = 0;
int16_t obs_x_field[144];
int16_t obs_y_field[144];

int16_t keeper_on_field[2];

// UNtuk debug obs di BS
uint8_t total_merged_obs;
int16_t obs_x_merge[75];
int16_t obs_y_merge[75];

typedef struct
{
    int16_t x;
    int16_t y;
} vec2_t;

vec2_t robot_passing_target;

uint8_t send_to_bs = 1;
;
double sleep_time = 0;
void send_data();

uint64_t get_time_now_usec()
{
    timeval tim;
    gettimeofday(&tim, NULL);
    return 1.0e6 * tim.tv_sec + tim.tv_usec;
}

/**
 * This function is to filter obstacles,
 * Each robot will send only obstacle they see
 */
// void obsFilter()
// {
//     mutex_obs.lock();
//     total_obs = 0;
//     memset(obs_x_field, 0, 288);
//     memset(obs_y_field, 0, 288);
//     for (uint16_t i = 0; i < 144; i++) {
//         if (obs_on_field[i] != 9999 && obs_on_field[i] != 0) {
//             obs_x_field[total_obs] = pos_x + obs_on_field[i] * cosf(i * 2.5 * DEG2RAD);
//             obs_y_field[total_obs] = pos_y + obs_on_field[i] * sinf(i * 2.5 * DEG2RAD);

//             total_obs++;
//         }
//     }

//     mutex_obs.unlock();
// }

/**
 * This function is to search index of interface in linux,
 * So, we not use IP, we just use Wifi-Interface
 * Use "iwconfig" linux command
 *
 * @return IF_index
 */
int if_NameToIndex(char *ifname, char *address)
{
    int fd;
    struct ifreq if_info;
    int if_index;

    memset(&if_info, 0, sizeof(if_info));
    strncpy(if_info.ifr_name, ifname, IFNAMSIZ - 1);

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        // PERRNO("socket");
        return -1;
    }
    if (ioctl(fd, SIOCGIFINDEX, &if_info) == -1)
    {
        // PERRNO("ioctl");
        close(fd);
        return -1;
    }
    if_index = if_info.ifr_ifindex;

    if (ioctl(fd, SIOCGIFADDR, &if_info) == -1)
    {
        // PERRNO("ioctl");
        close(fd);
        return -1;
    }

    close(fd);

    sprintf(address, "%d.%d.%d.%d\n",
            (int)((unsigned char *)if_info.ifr_hwaddr.sa_data)[2],
            (int)((unsigned char *)if_info.ifr_hwaddr.sa_data)[3],
            (int)((unsigned char *)if_info.ifr_hwaddr.sa_data)[4],
            (int)((unsigned char *)if_info.ifr_hwaddr.sa_data)[5]);
#ifdef COMM_DEBUG
    printf("** Using device %s -> %s\n", if_info.ifr_name, address);
#endif

    return if_index;
}
/**
 * This function is to open multicast socket
 *
 * @return zero on success
 */
int openSocket()
{
    struct sockaddr_in multicastAddress;
    struct ip_mreqn mreqn;
    struct ip_mreq mreq;
    int opt;
    char address[16]; // IPV4: xxx.xxx.xxx.xxx/0

    /* It use to receive data */
    bzero(&multicastAddress, sizeof(struct sockaddr_in));
    multicastAddress.sin_family = AF_INET;
    multicastAddress.sin_port = htons(nw_config.port);
    multicastAddress.sin_addr.s_addr = INADDR_ANY;

    /* It use to send data */
    bzero(&multiSocket.destAddress, sizeof(struct sockaddr_in));
    multiSocket.destAddress.sin_family = AF_INET;
    multiSocket.destAddress.sin_port = htons(nw_config.port);
    multiSocket.destAddress.sin_addr.s_addr = inet_addr(nw_config.multicast_ip);

    /* Set socket as Datagram (UDP) */
    if ((multiSocket.socketID = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        // PERRNO("socket");
        return -1;
    }

    /* Assign WiFi Interface to use multicast (send data to IP group) */
    memset((void *)&mreqn, 0, sizeof(mreqn));
    mreqn.imr_ifindex = if_NameToIndex(nw_config.iface, address); // use INADDR_ANY for default IF
    if ((setsockopt(multiSocket.socketID, IPPROTO_IP, IP_MULTICAST_IF, &mreqn, sizeof(mreqn))) == -1)
    {
        // PERRNO("setsockopt 1");
        return -1;
    }

    /* It allow to use more than one process that bind on the same UPD socket */
    opt = 1;
    if ((setsockopt(multiSocket.socketID, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) == -1)
    {
        // PERRNO("setsockopt 2");
        return -1;
    }

    memset((void *)&mreq, 0, sizeof(mreq));
    mreq.imr_multiaddr.s_addr = inet_addr(nw_config.multicast_ip);
    mreq.imr_interface.s_addr = inet_addr(address);

    /* Assign socket to multicast IP, so he will collect all message on them */
    if ((setsockopt(multiSocket.socketID, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq))) == -1)
    {
        // PERRNO("setsockopt 3");
        return -1;
    }

    /**
     * 0 for Disable reception of our own multicast
     * 1 for Receive our data
     * */
    opt = 0;
    if ((setsockopt(multiSocket.socketID, IPPROTO_IP, IP_MULTICAST_LOOP, &opt, sizeof(opt))) == -1)
    {
        // PERRNO("setsockopt disable loopback");
        return -1;
    }

    /**
     * 0 host
     * 1 subnet
     * 32 site
     * 64 region
     */
    opt = 1;
    if ((setsockopt(multiSocket.socketID, IPPROTO_IP, IP_MULTICAST_TTL, &opt, sizeof(opt))) == -1)
    {
        // PERRNO("setsockopt ttl");
        return -1;
    }

    /* Bind to socket, so i can receive data  */
    if (bind(multiSocket.socketID, (struct sockaddr *)&multicastAddress, sizeof(struct sockaddr_in)) == -1)
    {
        // PERRNO("bind");
        return -1;
    }

    return 0;
}

/* First time program is to close socket before open new socket */
void closeSocket()
{
    if (multiSocket.socketID != -1)
        shutdown(multiSocket.socketID, SHUT_RDWR);
}

void loadConfig()
{
    char *robot_num = getenv("ROBOT");
    // char* robot_num = "3";
    char config_file[100];
    std::string current_dir = ros::package::getPath("communication");
    sprintf(config_file, "%s/../../config/IRIS%s/static_conf.yaml", current_dir.c_str(), robot_num);
    printf("config file: %s\n", config_file);

    YAML::Node config = YAML::LoadFile(config_file);
    strcpy(nw_config.identifier, robot_num);
    strcpy(nw_config.iface, config["Multicast"]["iface"].as<std::string>().c_str());
    strcpy(nw_config.multicast_ip, config["Multicast"]["multicast_ip"].as<std::string>().c_str());
    nw_config.port = config["Multicast"]["port"].as<int>();

    printf("identifier: %s\n", nw_config.identifier);
    printf("iface: %s\n", nw_config.iface);
    printf("multicast_ip: %s\n", nw_config.multicast_ip);
    printf("port: %d\n", nw_config.port);
}

/* This will get data from master */
// void cllbck_mc_out(const iris_msgs::mc_outConstPtr& msg)
// {
//     // pos_x = msg->pos_x;
//     // pos_y = msg->pos_y;
//     // theta = msg->theta;
//     // ball_status = msg->ball_status;
//     // ball_x = msg->ball_x;
//     // ball_y = msg->ball_y;
//     // robot_cond = msg->robot_cond;
//     // pass_target = msg->pass_target;
//     // target_on_field = msg->target_on_field;
//     // ball_x_next = msg->ball_x_next;
//     // ball_y_next = msg->ball_y_next;
//     // robot_x_next = msg->pos_x_next;
//     // robot_y_next = msg->pos_y_next;
//     // kick_mode = msg->kick_mode;
//     // robot_passing_target.x = msg->passing_x;
//     // robot_passing_target.y = msg->passing_y;
//     send_to_bs = msg->send_to_bs;

//     int8_t identifier = nw_config.identifier[0] - '0';

//     obsFilter();

//     // Update other robots data_buffer
//     int64_t epoch_now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
//     memcpy(sh_pos_x + identifier, &pos_x, 2);
//     memcpy(sh_pos_y + identifier, &pos_y, 2);
//     memcpy(sh_theta + identifier, &theta, 2);
//     memcpy(sh_robot_cond + identifier, &robot_cond, 2);
//     memcpy(sh_target_on_field + identifier, &target_on_field, 1);
//     memcpy(sh_pass_target_num + identifier, &pass_target, 1);

//     sh_target_pos_x[identifier] = robot_passing_target.x;
//     sh_target_pos_y[identifier] = robot_passing_target.y;

//     mutex_obs.lock();
//     total_merged_obs = msg->total_merged_obs;
//     memset(obs_x_merge, 0, sizeof(obs_x_merge));
//     memset(obs_y_merge, 0, sizeof(obs_y_merge));
//     for (uint8_t it = 0; it < total_merged_obs; it++) {
//         obs_x_merge[it] = msg->obs_x_merged[it];
//         obs_y_merge[it] = msg->obs_y_merged[it];
//     }

//     sh_total_obs[identifier] = total_obs;
//     memcpy(sh_obs_x_field[identifier], obs_x_field, total_obs * 2);
//     memcpy(sh_obs_y_field[identifier], obs_y_field, total_obs * 2);
//     mutex_obs.unlock();

//     robot_epoch[identifier] = epoch_now;
//     // Update other robots data_valid
//     sh_data_valid[1] = (abs(epoch_now - robot_epoch[1]) <= delay_tolerance);
//     sh_data_valid[2] = (abs(epoch_now - robot_epoch[2]) <= delay_tolerance);
//     sh_data_valid[3] = (abs(epoch_now - robot_epoch[3]) <= delay_tolerance);
//     sh_data_valid[4] = (abs(epoch_now - robot_epoch[4]) <= delay_tolerance);
//     sh_data_valid[5] = (abs(epoch_now - robot_epoch[5]) <= delay_tolerance);

//     sh_data_valid[identifier] = 1;

//     // Sending datas to master node
//     iris_msgs::mc_in_rbts msg_mc_in_rbts;
//     msg_mc_in_rbts.data_valid.assign(sh_data_valid, sh_data_valid + 6);
//     msg_mc_in_rbts.pos_x.assign(sh_pos_x, sh_pos_x + 6);
//     msg_mc_in_rbts.pos_y.assign(sh_pos_y, sh_pos_y + 6);
//     msg_mc_in_rbts.theta.assign(sh_theta, sh_theta + 6);
//     msg_mc_in_rbts.robot_cond.assign(sh_robot_cond, sh_robot_cond + 6);
//     msg_mc_in_rbts.target_on_field.assign(sh_target_on_field, sh_target_on_field + 6);
//     msg_mc_in_rbts.obs_total.assign(sh_total_obs, sh_total_obs + 6);
//     msg_mc_in_rbts.obs_x_r1.assign(sh_obs_x_field[1], sh_obs_x_field[1] + sh_total_obs[1]);
//     msg_mc_in_rbts.obs_x_r2.assign(sh_obs_x_field[2], sh_obs_x_field[2] + sh_total_obs[2]);
//     msg_mc_in_rbts.obs_x_r3.assign(sh_obs_x_field[3], sh_obs_x_field[3] + sh_total_obs[3]);
//     msg_mc_in_rbts.obs_x_r4.assign(sh_obs_x_field[4], sh_obs_x_field[4] + sh_total_obs[4]);
//     msg_mc_in_rbts.obs_x_r5.assign(sh_obs_x_field[5], sh_obs_x_field[5] + sh_total_obs[5]);
//     msg_mc_in_rbts.obs_y_r1.assign(sh_obs_y_field[1], sh_obs_y_field[1] + sh_total_obs[1]);
//     msg_mc_in_rbts.obs_y_r2.assign(sh_obs_y_field[2], sh_obs_y_field[2] + sh_total_obs[2]);
//     msg_mc_in_rbts.obs_y_r3.assign(sh_obs_y_field[3], sh_obs_y_field[3] + sh_total_obs[3]);
//     msg_mc_in_rbts.obs_y_r4.assign(sh_obs_y_field[4], sh_obs_y_field[4] + sh_total_obs[4]);
//     msg_mc_in_rbts.obs_y_r5.assign(sh_obs_y_field[5], sh_obs_y_field[5] + sh_total_obs[5]);

//     msg_mc_in_rbts.passing_point_x.assign(sh_target_pos_x, sh_target_pos_x + 6);
//     msg_mc_in_rbts.passing_point_y.assign(sh_target_pos_y, sh_target_pos_y + 6);

//     msg_mc_in_rbts.pass_target_num.assign(sh_pass_target_num, sh_pass_target_num + 6);

//     pub_sh_robot_data.publish(msg_mc_in_rbts);
// }

void recv_cllbck(const ros::TimerEvent &)
{
    char recv_buf[512];
    struct sockaddr src_addr;
    socklen_t addr_len = sizeof(src_addr);

    //send_data();

    memset(recv_buf, 0, sizeof(recv_buf));

    /* Not Use MSG_DONTWAIT because of non-blocking */
    int16_t nrecv = recvfrom(recv_socket->socketID, recv_buf, 512, 0, &src_addr, &addr_len);

    if (nrecv > 0 && (recv_buf[3] >= '0' && recv_buf[3] <= '5') && recv_buf[0] == 'i')
    {
        /**
         * 0 is from Basestation
         * 1-5 is from robot based on his number
         */
        int8_t identifier = recv_buf[3] - '0';

        // Recv from Basestation
        if (identifier == 0)
        {
            iris_msgs::bs2pc msg_bs2pc;
            memcpy(&msg_bs2pc.mode, recv_buf + 4, 1);
            memcpy(&msg_bs2pc.command, recv_buf + 5, 1);
            memcpy(&msg_bs2pc.style, recv_buf + 6, 1);
            memcpy(&msg_bs2pc.target_manual_x, recv_buf + 11, 2);
            memcpy(&msg_bs2pc.target_manual_y, recv_buf + 13, 2);
            memcpy(&msg_bs2pc.target_manual_theta, recv_buf + 15, 2);
            memcpy(&msg_bs2pc.offset_robot_x, recv_buf + 17, 2);
            memcpy(&msg_bs2pc.offset_robot_y, recv_buf + 19, 2);
            memcpy(&msg_bs2pc.offset_robot_theta, recv_buf + 21, 2);
            memcpy(&msg_bs2pc.data_mux_1, recv_buf + 23, 2);
            memcpy(&msg_bs2pc.data_mux_2, recv_buf + 25, 2);
            memcpy(&msg_bs2pc.data_mux_control, recv_buf + 27, 2);
            memcpy(&msg_bs2pc.trim_translation_vel, recv_buf + 29 + atoi(nw_config.identifier) - 1, 1);
            memcpy(&msg_bs2pc.trim_rotation_vel, recv_buf + 34 + atoi(nw_config.identifier) - 1, 1);
            memcpy(&msg_bs2pc.trim_kick_power, recv_buf + 39 + atoi(nw_config.identifier) - 1, 1);
            memcpy(&msg_bs2pc.pass_counter, recv_buf + 44, 1);
            memcpy(&msg_bs2pc.index_obs_keeper, recv_buf + 45, 1);
            memcpy(&msg_bs2pc.index_obs_center, recv_buf + 46, 1);
            memcpy(&msg_bs2pc.index_obs_side, recv_buf + 47, 1);

            pub_bs2pc.publish(msg_bs2pc);

           // if (send_to_bs)
                send_data();
        }
        // Recv from another robots
        else
        {
            // Update other robots data_buffer
            // int64_t epoch_now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            // memcpy(robot_epoch + identifier, recv_buf + 4, 8);
            memcpy(sh_pos_x + identifier, recv_buf + 12, 2);
            memcpy(sh_pos_y + identifier, recv_buf + 14, 2);
            memcpy(sh_theta + identifier, recv_buf + 16, 2);
            memcpy(sh_robot_cond + identifier, recv_buf + 23, 2);

            iris_msgs::data_robots msg_data_robots;

            msg_data_robots.pose_x.assign(sh_pos_x, sh_pos_x + 4);
            msg_data_robots.pose_y.assign(sh_pos_y, sh_pos_y + 4);
            msg_data_robots.pose_theta.assign(sh_theta, sh_theta + 4);
            msg_data_robots.robot_condition.assign(robot_cond, robot_cond + 4);

            pub_data_robot.publish(msg_data_robots);
            // memcpy(sh_pass_target_num + identifier, recv_buf + 25, 1);
            // memcpy(sh_target_on_field + identifier, recv_buf + 26, 1);
            // memcpy(sh_total_obs + identifier, recv_buf + 27, 1);

            // static uint16_t counter = 28;
            // counter = 28;
        }

        //     robot_epoch[identifier] = epoch_now;

        //     for (int i = 1; i < 6; i++) {
        //         if (i == identifier) {
        //             counter = 28;
        //             for (int j = 0; j < sh_total_obs[i]; j++) {
        //                 memcpy(sh_obs_x_field[i] + j, recv_buf + counter, 2);
        //                 counter += 2;
        //                 memcpy(sh_obs_y_field[i] + j, recv_buf + counter, 2);
        //                 counter += 2;
        //             }
        //         }
        //     }
        //     counter += 16;
        //     memcpy(sh_target_pos_x + identifier, recv_buf + counter, 2);
        //     counter += 2;
        //     memcpy(sh_target_pos_y + identifier, recv_buf + counter, 2);
        //     counter += 2;

        //     // Sending datas to master node
        // iris_msgs::mc_in_rbts msg_mc_in_rbts;
        //     msg_mc_in_rbts.data_valid.assign(sh_data_valid, sh_data_valid + 6);
        //     msg_mc_in_rbts.pos_x.assign(sh_pos_x, sh_pos_x + 6);
        //     msg_mc_in_rbts.pos_y.assign(sh_pos_y, sh_pos_y + 6);
        //     msg_mc_in_rbts.theta.assign(sh_theta, sh_theta + 6);
        //     msg_mc_in_rbts.robot_cond.assign(sh_robot_cond, sh_robot_cond + 6);
        //     msg_mc_in_rbts.target_on_field.assign(sh_target_on_field, sh_target_on_field + 6);
        //     msg_mc_in_rbts.obs_total.assign(sh_total_obs, sh_total_obs + 6);
        //     msg_mc_in_rbts.obs_x_r1.assign(sh_obs_x_field[1], sh_obs_x_field[1] + sh_total_obs[1]);
        //     msg_mc_in_rbts.obs_x_r2.assign(sh_obs_x_field[2], sh_obs_x_field[2] + sh_total_obs[2]);
        //     msg_mc_in_rbts.obs_x_r3.assign(sh_obs_x_field[3], sh_obs_x_field[3] + sh_total_obs[3]);
        //     msg_mc_in_rbts.obs_x_r4.assign(sh_obs_x_field[4], sh_obs_x_field[4] + sh_total_obs[4]);
        //     msg_mc_in_rbts.obs_x_r5.assign(sh_obs_x_field[5], sh_obs_x_field[5] + sh_total_obs[5]);
        //     msg_mc_in_rbts.obs_y_r1.assign(sh_obs_y_field[1], sh_obs_y_field[1] + sh_total_obs[1]);
        //     msg_mc_in_rbts.obs_y_r2.assign(sh_obs_y_field[2], sh_obs_y_field[2] + sh_total_obs[2]);
        //     msg_mc_in_rbts.obs_y_r3.assign(sh_obs_y_field[3], sh_obs_y_field[3] + sh_total_obs[3]);
        //     msg_mc_in_rbts.obs_y_r4.assign(sh_obs_y_field[4], sh_obs_y_field[4] + sh_total_obs[4]);
        //     msg_mc_in_rbts.obs_y_r5.assign(sh_obs_y_field[5], sh_obs_y_field[5] + sh_total_obs[5]);

        //     msg_mc_in_rbts.passing_point_x.assign(sh_target_pos_x, sh_target_pos_x + 6);
        //     msg_mc_in_rbts.passing_point_y.assign(sh_target_pos_y, sh_target_pos_y + 6);

        //     msg_mc_in_rbts.pass_target_num.assign(sh_pass_target_num, sh_pass_target_num + 6);

        //     pub_sh_robot_data.publish(msg_mc_in_rbts);
        // }
    }
}

void send_with_delay_unblocking(int __fd, const void *__buf, size_t __n,
                                int __flags, __CONST_SOCKADDR_ARG __addr,
                                socklen_t __addr_len, useconds_t delay)
{
    usleep(delay);
    ssize_t data_sent = sendto(__fd, __buf, __n, __flags, __addr, __addr_len);
    return;
}

void send_data()
{
    // Get current unix epoch time

    int64_t epoch = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    // obsFilter();

    // Creating data-frame
    memcpy(send_buf, its, 3);
    memcpy(send_buf + 3, nw_config.identifier, 1);
    memcpy(send_buf + 4, &epoch, 8);
    memcpy(send_buf + 12, &pos_x, 2);
    memcpy(send_buf + 14, &pos_y, 2);
    memcpy(send_buf + 16, &theta, 2);
    memcpy(send_buf + 18, &ball_status, 1);
    memcpy(send_buf + 19, &ball_x, 2);
    memcpy(send_buf + 21, &ball_y, 2);
    memcpy(send_buf + 23, &robot_cond, 2);
    // memcpy(send_buf + 25, &pass_target, 1);
    // memcpy(send_buf + 26, &target_on_field, 1);
    // memcpy(send_buf + 27, &total_obs, 1);

    static uint16_t counter = 28;
    counter = 28;
    // mutex_obs.lock();
    // for (int i = 0; i < total_obs; i++) {
    //     memcpy(send_buf + counter, &obs_x_field[i], 2);
    //     counter += 2;
    //     memcpy(send_buf + counter, &obs_y_field[i], 2);
    //     counter += 2;
    // }

    memcpy(send_buf + counter, &battery, 4);
    counter += 4;

    // memcpy(send_buf + counter, keeper_on_field, 4);
    // counter += 4;

    // memcpy(send_buf + counter, &ball_x_next, 2);
    // counter += 2;
    // memcpy(send_buf + counter, &ball_y_next, 2);
    // counter += 2;

    // memcpy(send_buf + counter, &robot_x_next, 2);
    // counter += 2;
    // memcpy(send_buf + counter, &robot_y_next, 2);
    // counter += 2;
    // memcpy(send_buf + counter, &robot_passing_target.x, 2);
    // counter += 2;
    // memcpy(send_buf + counter, &robot_passing_target.y, 2);
    // counter += 2;
    // memcpy(send_buf + counter, &total_merged_obs, 1);
    // counter++;
    // for (uint8_t it = 0; it < total_merged_obs; it++) {
    //     memcpy(send_buf + counter, &obs_x_merge[it], 2);
    //     counter += 2;
    //     memcpy(send_buf + counter, &obs_y_merge[it], 2);
    //     counter += 2;
    // }

    // mutex_obs.unlock();

    // Set data actual size
    actual_data_size = counter;

    // Send over UDP multicast
    auto f1 = std::async(send_with_delay_unblocking, multiSocket.socketID, send_buf, actual_data_size, 0, (struct sockaddr *)&multiSocket.destAddress, sizeof(struct sockaddr), (useconds_t)(sleep_time * 1E6));
}

// void send_cllbck(const ros::SteadyTimerEvent&)
// {
//     if (one_cycle_send) {
//         // send_timer.stop();
//         return;
//     }
//     double qwe = ros::Time::now().toSec();
//     printf("cokk %lf\n", qwe);
//     usleep((useconds_t)(sleep_time * 1E6)); // Wait for time to send
//     double asd = ros::Time::now().toSec();
//     printf("ahhhh %lf\n", asd - qwe);
//     send_data();
//     one_cycle_send = 1;

// }

void CllbckSubPassingPoint(const geometry_msgs::Point32ConstPtr &msg)
{
    robot_passing_target.x = msg->x;
    robot_passing_target.y = msg->y;
}

void signal_catch(int sig)
{
    if (sig == SIGINT)
    {
        closeSocket();
        usleep(50000); // 50 ms
        ros::shutdown();
    }
}

void callbackSubscribeFinalPose(const geometry_msgs::Pose2DConstPtr &msg)
{
    pos_x = msg->x;
    pos_y = msg->y;
    theta = msg->theta;
}

void callbackSubscribeStm2pc(const iris_msgs::stm2pcConstPtr &msg)
{
    battery = msg->battery;
}

void callbackSubscribeBallStatus(const std_msgs::UInt8ConstPtr &msg)
{
    ball_status = msg->data;
}

void callbackSubscribeBallPos(const geometry_msgs::Pose2DConstPtr &msg)
{
    ball_x = msg->x;
    ball_y = msg->y;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "comm_multicast");
    ros::NodeHandle NH;
    ros::MultiThreadedSpinner spinner(0);

    loadConfig();

    memset(sh_data_valid, 0, 4);
    memset(sh_pos_x, 0, 8);
    memset(sh_pos_y, 0, 8);
    memset(sh_theta, 0, 8);
    memset(sh_robot_cond, 0, 8);

    closeSocket();
    if (openSocket() == -1)
    {
        // PERR("openMulticastSockett");
        ros::shutdown();
        return -1;
    }

    signal(SIGINT, signal_catch);

    sleep_time = atoi(nw_config.identifier) * UDP_TRAVEL_TIME_S;

    recv_socket = &multiSocket;

    send_buf[0] = 'q'; // Dummy data, indicates that this node doesn't get a valid data from robot
    send_buf[3] = '9'; // Dummy data, indicates that this node doesn't get a valid data from robot

    pub_bs2pc = NH.advertise<iris_msgs::bs2pc>("/communication/icast/bs2pc", 1);
    pub_data_robot = NH.advertise<iris_msgs::data_robots>("/communication/icast/data_robot", 1);
    sub_stm2pc = NH.subscribe<iris_msgs::stm2pc>("/hardware/stm2pc", 1, callbackSubscribeStm2pc);
    sub_final_pose = NH.subscribe<geometry_msgs::Pose2D>("/world_model/robot/pose", 1, callbackSubscribeFinalPose);
    sub_ball_status = NH.subscribe<std_msgs::UInt8>("/world_model/ball/status", 1, callbackSubscribeBallStatus);
    sub_ball_pos = NH.subscribe<geometry_msgs::Pose2D>("/world_model/ball/final_pos", 1, callbackSubscribeBallPos);
    recv_timer = NH.createTimer(ros::Duration(0.0001), recv_cllbck);

    spinner.spin();

    recv_timer.stop();
    send_timer.stop();

    return 0;
}
