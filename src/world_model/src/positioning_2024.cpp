#include "boost/thread/mutex.hpp"
#include "iris_msgs/bs2pc.h"
#include "iris_msgs/data_robots.h"
#include "ros/ros.h"
#include "sensor_msgs/PointCloud.h"
#include "std_msgs/Float32MultiArray.h"
#include "std_msgs/UInt8.h"
#include "world_model/enums.hpp"
#include "world_model/utils.hpp"
// #include <cv_bridge/cv_bridge.h>
#include <geometry_msgs/Pose2D.h>
// #include <image_transport/image_transport.h>
#include <opencv2/opencv.hpp>

typedef struct
{
    float x;
    float y;
} point_t;

typedef struct
{
    uint8_t is_iam_controlled_by_bs;
    uint8_t is_iam_send_data_to_bs;
    uint8_t command;
    uint8_t mode;
    uint8_t manual_mode;
    uint8_t auto_calibration;
    uint8_t style;
    uint16_t data_mux1;
    uint16_t data_mux2;
    uint16_t mux_control;
    int16_t obs_bs_regional[3];
    double last_time_connected;

    uint8_t n_active_robot; // How many active robots?
    uint8_t n_nearest_ball; // Who has closest to ball?
    uint8_t n_has_ball;     // Who has ball?
    uint8_t n_pass;         // Who is passing?
    uint8_t n_pass_target;  // Who is receiver of passing?

    uint8_t n_robot_att;    // Number robot of attacker role
    uint8_t n_robot_assist; // Number robot of assist role
    uint8_t n_robot_gk;     // Number robot of Goalkeeper role
    uint8_t n_robot_def_1;  // Number robot of Defender1 role
    uint8_t n_robot_def_2;  // Number robot of Defender2 role
} bs_t;

typedef struct
{
    double x;
    double y;
    uint8_t status;
} ball_t;

typedef struct
{
    float x;
    float y;
    float distance;
} obs_t;

#define FIELD_Y_0 0
#define FIELD_X_0 0
#define FIELD_Y_1 1200
#define FIELD_X_1 800

#define FIELD_Y_1_2 (FIELD_Y_1 / 2.f)
#define FIELD_X_1_2 (FIELD_X_1 / 2.f)

#define FIELD_Y_1_3 (FIELD_Y_1 / 3.f)
#define FIELD_X_1_3 (FIELD_X_1 / 3.f)

#define FIELD_Y_1_4 (FIELD_Y_1 / 4.f)
#define FIELD_X_1_4 (FIELD_X_1 / 4.f)

#define FIELD_Y_1_5 (FIELD_Y_1 / 5.f)
#define FIELD_X_1_5 (FIELD_X_1 / 5.f)

#define FIELD_Y_1_6 (FIELD_Y_1 / 6.f)
#define FIELD_X_1_6 (FIELD_X_1 / 6.f)

#define FIELD_Y_2_3 (FIELD_Y_1 * 2.f / 3.f)
#define FIELD_X_2_3 (FIELD_X_1 * 2.f / 3.f)

#define FIELD_Y_3_4 (FIELD_Y_1 * 3.f / 4.f)
#define FIELD_X_3_4 (FIELD_X_1 * 3.f / 4.f)

#define FIELD_X_4_5 (FIELD_X_1 * 4.f / 5.f)
#define FIELD_Y_4_5 (FIELD_Y_1 * 4.f / 5.f)

#define FIELD_X_5_6 (FIELD_X_1 * 5.f / 6.f)
#define FIELD_Y_5_6 (FIELD_Y_1 * 5.f / 6.f)

// #define RAD2DEG *57.295780
// #define DEG2RAD *0.017452925

typedef struct
{
    int a;
    float b;
} int_float_pair_t;

using namespace std;

//===========================

ros::Timer tim_update;

ros::Subscriber sub_obs_pcl;
ros::Subscriber sub_obs;
ros::Subscriber sub_bs_data;
ros::Subscriber sub_shared_data;
ros::Subscriber sub_bs2pc;
ros::Subscriber sub_ball_pos;
ros::Subscriber sub_ball_status;

ros::Publisher pub_titik_final;

//==============================

int robot_saya_num;

//===== NEW
// Basestation data
bs_t bs_data;

// Robot Shared Data
shrobot_data_t shared_robot_data[4];

int8_t robot_saya_role = 2;

float teman_x;
float teman_y;

float pos_robot_x;
float pos_robot_y;

int16_t game_status;

ball_t ball;

obs_t obs_terdekat;

const int16_t gawang_saya_kanan_x = 450;
const int16_t gawang_saya_kanan_y = 0;
const int16_t gawang_saya_kiri_x = 350;
const int16_t gawang_saya_kiri_y = 0;
const int16_t gawang_lawan_x = 400;
const int16_t gawang_lawan_y = 1200;

std::vector<uint16_t> obs_on_field;
std::vector<cv::Point2f> obs_merged;
std::vector<point_t> obs_pcl;

boost::mutex mutex_obs_merged;
boost::mutex mutex_obs_pcl;

uint8_t ball_status_buffer = 0;

//=========================

vector<cv::Point2f> titik_peluang;
vector<float> titik_peluang_peluang;

float titik_final[2];

//==========================

void callback_update(const ros::TimerEvent &);
void callback_sub_obs_merged(const sensor_msgs::PointCloudConstPtr &msg);
void callback_sub_bs_data(const iris_msgs::bs2pcConstPtr &msg);
void callback_sub_shared_data(const iris_msgs::data_robotsConstPtr &msg);
void callback_sub_final_ball(const geometry_msgs::Pose2DConstPtr &msg);
void callback_sub_ball_status(const std_msgs::UInt8ConstPtr &msg);

void readU16Bit(uint8_t *dst, const uint16_t *src, uint8_t total_bit, uint8_t offset_bit);
void make_layer_1();
void make_layer_2_dan_3();
void make_layer_2_dan_3_away();
void make_layer_4();
void make_layer_3(float obs_x, float obs_y);

void append_titik_peluang(float x, float y, float peluang = 99);
cv::Point get_max_peluang();
vector<cv::Point> make_tangentCircle_polygon(float x1, float y1, float x2, float y2, float radius, uint8_t full);
vector<float> tangentCircle(const cv::Point2f &mouse, const cv::Point2f &center, double radius);

int main(int argc, char **argv)
{
    ros::init(argc, argv, "positioning");
    ros::NodeHandle NH;
    ros::MultiThreadedSpinner spinner(0);

    //=================== PREPARE

    robot_saya_num = atoi(getenv("ROBOT"));
    memset(shared_robot_data, 0, sizeof(shared_robot_data));

    //==================================

    // tim_update = NH.createTimer(ros::Duration(0.5), callback_update);
    sub_obs = NH.subscribe("/world_model/obs/shared_all", 1, callback_sub_obs_merged);
    sub_shared_data = NH.subscribe("/communication/icast/data_robot", 1, callback_sub_shared_data);
    sub_bs2pc = NH.subscribe("/communication/icast/bs2pc", 1, callback_sub_bs_data);
    sub_ball_pos = NH.subscribe("/world_model/ball/final_pos", 1, callback_sub_final_ball);
    sub_ball_status = NH.subscribe("/world_model/ball/status", 1, callback_sub_ball_status);

    pub_titik_final = NH.advertise<std_msgs::Float32MultiArray>("/world_model/positioning/titik_final_positioning", 1);

    spinner.spin();
    tim_update.stop();

    return 0;
}

//==========================================

void callback_sub_ball_status(const std_msgs::UInt8ConstPtr &msg)
{
    ball.status = msg->data;
    ball_status_buffer = ball.status;
    // printf("CLLBCL: %d\n", ball.status);
}

void callback_sub_final_ball(const geometry_msgs::Pose2DConstPtr &msg)
{
    ball.x = msg->x;
    ball.y = msg->y;
}

void callback_sub_obs_merged(const sensor_msgs::PointCloudConstPtr &msg)
{
    // mutex_obs_merged.lock();
    obs_merged.clear();
    for (int i = 0; i < msg->points.size(); i++)
    {
        cv::Point2f temp;
        temp.x = msg->points[i].x;
        temp.y = msg->points[i].y;
        obs_merged.push_back(temp);
    }
    // mutex_obs_merged.unlock();

    float min_distance = FLT_MAX;
    for (int i = 0; i < obs_merged.size(); i++)
    {
        if (obs_merged[i].y < pos_robot_y && obs_merged[i].y > teman_y)
        {
            if (pythagoras(pos_robot_x, pos_robot_y, obs_merged[i].x, obs_merged[i].y) < min_distance)
            {
                min_distance = pythagoras(pos_robot_x, pos_robot_y, obs_merged[i].x, obs_merged[i].y);
                obs_terdekat.x = obs_merged[i].x;
                obs_terdekat.y = obs_merged[i].y;
                obs_terdekat.distance = min_distance;
            }
        }
    }

    // static uint8_t counter = 0;

    // if (counter++ <= 2) {
    //     return;
    // }
    // counter = 0;

// KEYBOARD UNTUK SIMULASI
#ifndef KEYBOARD
// #define KEYBOARD
#endif
#ifdef KEYBOARD
    if (kbhit() > 0)
    {
        char c = std::getchar();

        switch (c)
        {
        case '1':
            robot_saya_role = 1;
            bs_data.n_robot_def_1 = 3;
            bs_data.n_robot_att = 2;

            break;
        case '2':
            robot_saya_role = 2;
            bs_data.n_robot_def_1 = 3;
            bs_data.n_robot_att = 2;
            break;
        case '3':
            ball.status = 0;
            break;
        case '4':
            ball.status = 2;
            break;
        case '5':
            ball.x = 200;
            ball.y = 600;
            break;
        case '6':
            ball.x = 400;
            ball.y = 600;
            break;
        case '7':
            ball.x = 600;
            ball.y = 600;
            break;
        case '8':
            ball.x = 0;
            ball.y = 1200;
            break;
        case '9':
            ball.x = 800;
            ball.y = 1200;
            break;
        case '0':
            ball.x = 0;
            ball.y = 0;
            break;
        case '-':
            ball.x = 800;
            ball.y = 0;
            break;
        default:
            game_status = c;
            break;
        }

        // shared_robot_data[2].pose.x = 600;
        // shared_robot_data[2].pose.y = 900;
        // shared_robot_data[3].pose.x = 600;
        // shared_robot_data[3].pose.y = 200;
    }
#endif
    //========================

    make_layer_1();

    switch (game_status)
    {
    case status_preparation_kickoff_away:
    case status_preparation_freekick_away:
    case status_preparation_goalkick_away:
    case status_preparation_throwin_away:
    case status_preparation_cornerkick_away:
        make_layer_2_dan_3_away();
        break;

    default:
        make_layer_2_dan_3();
        // make_layer_2_dan_3_away();

        break;
    }

    cv::Point ret_buffer = get_max_peluang();
    titik_final[0] = ret_buffer.x;
    titik_final[1] = ret_buffer.y;

    std_msgs::Float32MultiArray msg_titik_final;
    msg_titik_final.data.push_back(titik_final[0]);
    msg_titik_final.data.push_back(titik_final[1]);
    pub_titik_final.publish(msg_titik_final);
}

void callback_sub_bs_data(const iris_msgs::bs2pcConstPtr &msg)
{
    uint8_t robot_num = atoi(getenv("ROBOT"));

    bs_data.command = msg->command;

    readU16Bit(&bs_data.n_active_robot, &msg->data_mux_1, 3, 0);
    readU16Bit(&bs_data.n_nearest_ball, &msg->data_mux_1, 3, 3);
    readU16Bit(&bs_data.n_has_ball, &msg->data_mux_1, 3, 6);
    readU16Bit(&bs_data.n_pass, &msg->data_mux_1, 3, 9);
    readU16Bit(&bs_data.n_pass_target, &msg->data_mux_1, 3, 12);

    readU16Bit(&shared_robot_data[1].role, &msg->data_mux_2, 3, 0);
    readU16Bit(&shared_robot_data[2].role, &msg->data_mux_2, 3, 3);
    readU16Bit(&shared_robot_data[3].role, &msg->data_mux_2, 3, 6);
    readU16Bit(&shared_robot_data[4].role, &msg->data_mux_2, 3, 9);
    readU16Bit(&shared_robot_data[5].role, &msg->data_mux_2, 3, 12);

    for (uint8_t i = 0; i < 4; i++)
    {
        switch (shared_robot_data[i].role)
        {
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
            bs_data.n_robot_gk = i;
            break;
        }
    }

    robot_saya_role = shared_robot_data[robot_num].role;

    // Proses Command BS
    static uint8_t prev_BS_cmd = 0;

    static const uint8_t robot_num_bin[6] = {0b00000, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000};

    bs_data.is_iam_controlled_by_bs = (msg->data_mux_control & (robot_num_bin[robot_num])) >> (robot_num - 1);

    if (bs_data.is_iam_controlled_by_bs)
    {
        if (prev_BS_cmd != bs_data.command)
        {
            if (bs_data.command == 's')
            {
                if (game_status > 0 && game_status <= 127)
                    game_status += 128;
            }
            else if (bs_data.command == 'S')
            {
                if (game_status > 127 && game_status <= 255)
                    game_status -= 128;
            }
            else if (bs_data.command != 126)
            {
                game_status = bs_data.command;
            }

            // robot_base_action = (!(prev_BS_cmd == 126 && bs_data.command == 'S') && bs_data.command != ' ') * (bs_data.command != 126) | (bs_data.command == status_park);
        }
        prev_BS_cmd = bs_data.command;
    }
    else
    {
        prev_BS_cmd = 126;
    }
}
void callback_sub_shared_data(const iris_msgs::data_robotsConstPtr &msg)
{
    for (uint8_t i = 1; i < msg->pose_x.size(); i++)
    {
        shared_robot_data[i].pose.x = msg->pose_x.at(i);
    }
    for (uint8_t i = 1; i < msg->pose_y.size(); i++)
    {
        shared_robot_data[i].pose.y = msg->pose_y.at(i);
    }
    for (uint8_t i = 1; i < msg->pose_theta.size(); i++)
    {
        shared_robot_data[i].pose.theta = msg->pose_theta.at(i);
    }
}

//==================================================

void callback_update(const ros::TimerEvent &)
{
}

//===================================================

void make_layer_1()
{
    if (robot_saya_role == 1)
    {
        // printf("ROBOT: %d | %.2f %.2f\n", robot_saya_role, shared_robot_data[bs_data.n_robot_def_1].pose.x, shared_robot_data[bs_data.n_robot_def_1].pose.y);
        teman_x = shared_robot_data[bs_data.n_robot_def_1].pose.x;
        teman_y = shared_robot_data[bs_data.n_robot_def_1].pose.y;
    }
    else
    {
        // printf("ROBOT: %d | %.2f %.2f\n", robot_saya_role, shared_robot_data[bs_data.n_robot_att].pose.x, shared_robot_data[bs_data.n_robot_att].pose.y);
        teman_x = shared_robot_data[bs_data.n_robot_att].pose.x;
        teman_y = shared_robot_data[bs_data.n_robot_att].pose.y;
    }

    // printf("halo: %.2f %.2f\n", teman_x, teman_y);

    titik_peluang.clear();
    titik_peluang_peluang.clear();

    switch (game_status)
    {
    case status_preparation_goalkick_home:
    case status_preparation_freekick_home:
        if (teman_x < FIELD_X_1_2)
        {
            // goalkick kiri
            append_titik_peluang(158, 929, 99);
            append_titik_peluang(64, 888, 98);
            append_titik_peluang(312, 896, 97);
            append_titik_peluang(419, 879, 96);
            append_titik_peluang(514, 855, 95);
        }
        else
        {
            // goalkick kanan
            append_titik_peluang(557, 907, 99);
            append_titik_peluang(692, 934, 98);
            append_titik_peluang(481, 877, 97);
            append_titik_peluang(400, 846, 96);
            append_titik_peluang(282, 806, 95);
        }

        break;
    case status_preparation_cornerkick_home:
    case status_preparation_throwin_home:
        if (teman_x < FIELD_X_1_2)
        {
            // throwing kiri
            append_titik_peluang(731, 783, 99);
            append_titik_peluang(721, 680, 98);
            append_titik_peluang(734, 860, 97);
            append_titik_peluang(750, 971, 96);
            append_titik_peluang(728, 1105, 95);
        }
        else
        {
            // throwin kanan
            append_titik_peluang(800 - 731, 783, 99);
            append_titik_peluang(800 - 721, 680, 98);
            append_titik_peluang(800 - 734, 860, 97);
            append_titik_peluang(800 - 750, 971, 96);
            append_titik_peluang(800 - 728, 1105, 95);
        }

        break;
    case status_preparation_kickoff_away:
        if (robot_saya_role == 1)
        {
            append_titik_peluang(200, 450, 94);
            append_titik_peluang(200, 525, 95);
            append_titik_peluang(200, 600, 96);
            append_titik_peluang(300, 450, 97);
            append_titik_peluang(300, 525, 98);
            append_titik_peluang(300, 600, 99);
            append_titik_peluang(400, 450, 100);
            append_titik_peluang(400, 525, 101);
            append_titik_peluang(400, 600, 100);
            append_titik_peluang(500, 450, 99);
            append_titik_peluang(500, 525, 98);
            append_titik_peluang(500, 600, 97);
            append_titik_peluang(600, 450, 96);
            append_titik_peluang(600, 525, 95);
            append_titik_peluang(600, 600, 94);
        }
        else
        {
            append_titik_peluang(200, 450 - 200, 94);
            append_titik_peluang(200, 525 - 200, 95);
            append_titik_peluang(200, 600 - 200, 96);
            append_titik_peluang(300, 450 - 200, 97);
            append_titik_peluang(300, 525 - 200, 98);
            append_titik_peluang(300, 600 - 200, 99);
            append_titik_peluang(400, 450 - 200, 100);
            append_titik_peluang(400, 525 - 200, 101);
            append_titik_peluang(400, 600 - 200, 100);
            append_titik_peluang(500, 450 - 200, 99);
            append_titik_peluang(500, 525 - 200, 98);
            append_titik_peluang(500, 600 - 200, 97);
            append_titik_peluang(600, 450 - 200, 96);
            append_titik_peluang(600, 525 - 200, 95);
            append_titik_peluang(600, 600 - 200, 94);
        }
        break;
    case status_preparation_goalkick_away:
    case status_preparation_freekick_away:
        if (robot_saya_role == 1)
        {
            append_titik_peluang(200, 450, 94);
            append_titik_peluang(200, 525, 95);
            append_titik_peluang(200, 600, 96);
            append_titik_peluang(300, 450, 97);
            append_titik_peluang(300, 525, 98);
            append_titik_peluang(300, 600, 99);
            append_titik_peluang(400, 450, 100);
            append_titik_peluang(400, 525, 101);
            append_titik_peluang(400, 600, 100);
            append_titik_peluang(500, 450, 99);
            append_titik_peluang(500, 525, 98);
            append_titik_peluang(500, 600, 97);
            append_titik_peluang(600, 450, 96);
            append_titik_peluang(600, 525, 95);
            append_titik_peluang(600, 600, 94);
        }
        else
        {
            append_titik_peluang(200, 450 - 200, 94);
            append_titik_peluang(200, 525 - 200, 95);
            append_titik_peluang(200, 600 - 200, 96);
            append_titik_peluang(300, 450 - 200, 97);
            append_titik_peluang(300, 525 - 200, 98);
            append_titik_peluang(300, 600 - 200, 99);
            append_titik_peluang(400, 450 - 200, 100);
            append_titik_peluang(400, 525 - 200, -1);
            append_titik_peluang(400, 600 - 200, 100);
            append_titik_peluang(500, 450 - 200, 99);
            append_titik_peluang(500, 525 - 200, 98);
            append_titik_peluang(500, 600 - 200, 97);
            append_titik_peluang(600, 450 - 200, 96);
            append_titik_peluang(600, 525 - 200, 95);
            append_titik_peluang(600, 600 - 200, 94);
        }

        break;
    case status_preparation_cornerkick_away:
    case status_preparation_throwin_away:
        if (robot_saya_role == 1)
        {
            append_titik_peluang(600, 200, 99);
            append_titik_peluang(660, 275, 98);
            append_titik_peluang(538, 286, 97);
            append_titik_peluang(517, 211, 96);
            append_titik_peluang(588, 371, 95);
            append_titik_peluang(505, 324, 94);
        }
        else
        {
            append_titik_peluang(800 - 600, 200, 99);
            append_titik_peluang(800 - 660, 275, 98);
            append_titik_peluang(800 - 538, 286, 97);
            append_titik_peluang(800 - 517, 211, 96);
            append_titik_peluang(800 - 588, 371, 95);
            append_titik_peluang(800 - 505, 324, 94);
        }

        break;
    }

    titik_peluang.clear();
    // append_titik_peluang(200, 450 - 200, 94 - 90);
    // append_titik_peluang(200, 525 - 200, 95 - 90);
    // append_titik_peluang(200, 600 - 200, 96 - 90);
    // append_titik_peluang(300, 450 - 200, 97 - 90);
    // append_titik_peluang(300, 525 - 200, 98 - 90);
    // append_titik_peluang(300, 600 - 200, 99 - 90);
    // append_titik_peluang(400, 450 - 200, 100 - 90);
    // append_titik_peluang(400, 525 - 200, -1);
    // append_titik_peluang(400, 600 - 200, 100 - 90);
    // append_titik_peluang(500, 450 - 200, 99 - 90);
    // append_titik_peluang(500, 525 - 200, 98 - 90);
    // append_titik_peluang(500, 600 - 200, 97 - 90);
    // append_titik_peluang(600, 450 - 200, 96 - 90);
    // append_titik_peluang(600, 525 - 200, 95 - 90);
    // append_titik_peluang(600, 600 - 200, 94 - 90);

    append_titik_peluang(158, 929, 99);
    append_titik_peluang(64, 888, 98);
    append_titik_peluang(312, 896, 97);
    append_titik_peluang(419, 879, 96);
    append_titik_peluang(514, 855, 95);
}
void make_layer_2_dan_3()
{
    if (robot_saya_role == 1)
    {
        teman_x = shared_robot_data[bs_data.n_robot_def_1].pose.x;
        teman_y = shared_robot_data[bs_data.n_robot_def_1].pose.y;
    }
    else
    {
        teman_x = shared_robot_data[bs_data.n_robot_att].pose.x;
        teman_y = shared_robot_data[bs_data.n_robot_att].pose.y;
    }

    teman_x = 570;
    teman_y = 180;

    std::vector<int_float_pair_t> something;
    for (size_t peluang_it = 0; peluang_it < titik_peluang.size(); peluang_it++)
    {
        double buffer_jarak_max = 0;
        uint8_t obs_ditemukan = 0;
        for (size_t obs_it = 0; obs_it < obs_merged.size(); obs_it++)
        {
            float jarak_teman_ke_obs = pythagoras(teman_x, teman_y, obs_merged[obs_it].x, obs_merged[obs_it].y);
            float radius_tangentCircle = 70;
            // if (jarak_teman_ke_obs > 400)
            //     radius_tangentCircle = 80;
            // else if (jarak_teman_ke_obs > 300)
            //     radius_tangentCircle = 65;
            // else if (jarak_teman_ke_obs > 200)
            //     radius_tangentCircle = 50;
            // else
            //     radius_tangentCircle = 26;

            vector<cv::Point> blocking_teman_ke_obs = make_tangentCircle_polygon(teman_x, teman_y, obs_merged[obs_it].x, obs_merged[obs_it].y, radius_tangentCircle, 0);
            double jarak_blocking_teman_ke_obs = cv::pointPolygonTest(blocking_teman_ke_obs, titik_peluang[peluang_it], true);

            vector<cv::Point> blocking_gawang_ke_obs = make_tangentCircle_polygon(gawang_lawan_x, gawang_lawan_y, obs_merged[obs_it].x, obs_merged[obs_it].y, 25, 0);
            double jarak_blocking_gawang_ke_obs = cv::pointPolygonTest(blocking_teman_ke_obs, titik_peluang[peluang_it], true);

            if (jarak_blocking_teman_ke_obs > buffer_jarak_max)
            {
                buffer_jarak_max = jarak_blocking_teman_ke_obs;
            }

            // printf("Hello: %.2lf %.2lf\n", jarak_blocking_teman_ke_obs, buffer_jarak_max);

            // titik peluang berada di dalam polygon blocking
            if (buffer_jarak_max > 0)
            {
                obs_ditemukan = 1;
                titik_peluang_peluang[peluang_it] = buffer_jarak_max;
            }
        }

        if (obs_ditemukan == 1)
        {
            int_float_pair_t temp;
            temp.a = peluang_it;
            temp.b = titik_peluang_peluang[peluang_it];
            something.push_back(temp);
        }
    }

    float max_value = 0;
    for (size_t i = 0; i < something.size(); i++)
    {
        if (something[i].b > max_value)
        {
            max_value = something[i].b;
        }
    }

    for (size_t i = 0; i < something.size(); i++)
    {
        titik_peluang_peluang[something[i].a] = max_value - titik_peluang_peluang[something[i].a];
    }
}
void make_layer_2_dan_3_away()
{
    if (robot_saya_role == 1)
    {
        teman_x = shared_robot_data[bs_data.n_robot_def_1].pose.x;
        teman_y = shared_robot_data[bs_data.n_robot_def_1].pose.y;
    }
    else
    {
        teman_x = shared_robot_data[bs_data.n_robot_att].pose.x;
        teman_y = shared_robot_data[bs_data.n_robot_att].pose.y;
    }

    for (size_t peluang_it = 0; peluang_it < titik_peluang.size(); peluang_it++)
    {
        if (fabsf(titik_peluang_peluang[peluang_it] - -1) < FLT_EPSILON)
        {
            continue;
        }
        if (ball.status > 0)
        {
            vector<cv::Point> blocking_teman_ke_obs = make_tangentCircle_polygon(gawang_saya_kanan_x, gawang_saya_kanan_y, ball.x, ball.y, 150, 2);
            double jarak_blocking_teman_ke_obs = cv::pointPolygonTest(blocking_teman_ke_obs, titik_peluang[peluang_it], true);

            // titik peluang berada di dalam atau pinggir polygon blocking
            if (jarak_blocking_teman_ke_obs >= 0)
            {
                titik_peluang_peluang[peluang_it] = jarak_blocking_teman_ke_obs;
            }

            float jarak_titik_ke_bola = pythagoras(titik_peluang[peluang_it].x, titik_peluang[peluang_it].y, ball.x, ball.y);
            if (jarak_titik_ke_bola < 200)
            {
                titik_peluang_peluang[peluang_it] = 0;
            }
        }
        else
        {
            for (size_t obs_it = 0; obs_it < obs_merged.size(); obs_it++)
            {
                vector<cv::Point> blocking_teman_ke_obs = make_tangentCircle_polygon(gawang_saya_kanan_x, gawang_saya_kanan_y, obs_merged[obs_it].x, obs_merged[obs_it].y, 150, 2);
                double jarak_blocking_teman_ke_obs = cv::pointPolygonTest(blocking_teman_ke_obs, titik_peluang[peluang_it], true);

                // titik peluang berada di dalam atau pinggir polygon blocking
                if (jarak_blocking_teman_ke_obs >= 0)
                {
                    titik_peluang_peluang[peluang_it] = jarak_blocking_teman_ke_obs;
                }

                float jarak_titik_ke_obs = pythagoras(titik_peluang[peluang_it].x, titik_peluang[peluang_it].y, obs_merged[obs_it].x, obs_merged[obs_it].y);
                if (jarak_titik_ke_obs < 70)
                {
                    titik_peluang_peluang[peluang_it] = 0;
                }
            }
        }

        // printf("%d: %.2f %.2f %.2f\n", peluang_it, titik_peluang[peluang_it].x, titik_peluang[peluang_it].y, titik_peluang_peluang[peluang_it]);
    }
    // printf("=====================\n");
}

vector<cv::Point> make_tangentCircle_polygon(float x1, float y1, float x2, float y2, float radius, uint8_t full)
{
    vector<float> tangent_titik_ke_titik;

    tangent_titik_ke_titik = tangentCircle(cv::Point2f(x1, y1), cv::Point2f(x2, y2), radius);

    float x_tembok_1, x_tembok_2;
    float y_tembok_1, y_tembok_2;

    {
        float dx = tangent_titik_ke_titik[0] - x1;
        float dy = tangent_titik_ke_titik[1] - y1;

        if (dx >= 0 && dx < FLT_EPSILON)
            dx = FLT_EPSILON;
        else if (dx < -0 && dx > -FLT_EPSILON)
            dx = -FLT_EPSILON;

        float m = dy / dx;
        float c = y1 - m * x1;

        if (m <= 1 && m >= -1)
        {
            x_tembok_1 = dx > 0 ? 800 : 0;
            y_tembok_1 = m * x_tembok_1 + c;
        }
        else
        {
            y_tembok_1 = dy > 0 ? 1200 : 0;
            x_tembok_1 = (y_tembok_1 - c) / m;
        }
    }

    {
        float dx = tangent_titik_ke_titik[2] - x1;
        float dy = tangent_titik_ke_titik[3] - y1;

        if (dx >= 0 && dx < FLT_EPSILON)
            dx = FLT_EPSILON;
        else if (dx < -0 && dx > -FLT_EPSILON)
            dx = -FLT_EPSILON;

        float m = dy / dx;
        float c = y1 - m * x1;

        if (m <= 1 && m >= -1)
        {
            x_tembok_2 = dx > 0 ? 800 : 0;
            y_tembok_2 = m * x_tembok_2 + c;
        }
        else
        {
            y_tembok_2 = dy > 0 ? 1200 : 0;
            x_tembok_2 = (y_tembok_2 - c) / m;
        }
    }

    vector<cv::Point> pts;
    if (full != 2)
    {
        pts.push_back(cv::Point(tangent_titik_ke_titik[0], tangent_titik_ke_titik[1]));
        if (full == 1)
            pts.push_back(cv::Point(x1, y1));
        pts.push_back(cv::Point(tangent_titik_ke_titik[2], tangent_titik_ke_titik[3]));
        pts.push_back(cv::Point(x_tembok_2, y_tembok_2));
        pts.push_back(cv::Point(x_tembok_1, y_tembok_1));
    }
    else
    {
        pts.push_back(cv::Point(tangent_titik_ke_titik[0], tangent_titik_ke_titik[1]));
        pts.push_back(cv::Point(x1, y1));
        pts.push_back(cv::Point(tangent_titik_ke_titik[2], tangent_titik_ke_titik[3]));
    }

    return pts;
}

vector<float> tangentCircle(const cv::Point2f &mouse, const cv::Point2f &center, double radius)
{
    vector<float> points(4, 0);

    // Line between point mouse and center
    float directLine = sqrt(pow((mouse.x - center.x), 2) + pow((mouse.y - center.y), 2));

    // angle of point mouse and center
    float dx = center.x - mouse.x;
    float dy = center.y - mouse.y;
    float directAngle = atan2f(dy, dx);

    // angle between radius and directLine
    float theta = acos(radius / directLine);

    // direction angle of point tangent 1 and center
    float d1 = directAngle + theta;

    // direction angle of point tangent 2 and center
    float d2 = directAngle - theta;

    points[0] = (int)(center.x + radius * cos(d1));
    points[1] = (int)(center.y + radius * sin(d1));

    points[2] = (int)(center.x + radius * cos(d2));
    points[3] = (int)(center.y + radius * sin(d2));

    return points;
}

cv::Point get_max_peluang()
{
    float max_value = 0;
    int max_idx = -1;
    int default_idx = -1;
    for (size_t i = 0; i < titik_peluang_peluang.size(); i++)
    {
        if (fabsf(titik_peluang_peluang[i] - -1) < FLT_EPSILON)
        {
            default_idx = i;
            continue;
        }

        // printf("%d: %.2f %.2f %.2f\n", i, titik_peluang[i].x, titik_peluang[i].y, titik_peluang_peluang[i]);

        if (titik_peluang_peluang[i] > max_value)
        {
            max_value = titik_peluang_peluang[i];
            max_idx = i;
        }
    }

    // printf("=======================\n");

    if (max_idx == -1)
    {
        if (default_idx >= 0)
        {
            cv::Point pt;
            pt.x = titik_peluang[default_idx].x;
            pt.y = titik_peluang[default_idx].y;
            return pt;
        }
        else
        {
            cv::Point pt;
            pt.x = FIELD_X_1_2;
            pt.y = FIELD_Y_1_2;
            return pt;
        }
    }

    return titik_peluang[max_idx];
}

void append_titik_peluang(float x, float y, float peluang)
{
    cv::Point2f pt;
    pt.x = x;
    pt.y = y;
    titik_peluang.push_back(pt);
    titik_peluang_peluang.push_back(peluang);
}

void readU16Bit(uint8_t *dst, const uint16_t *src, uint8_t total_bit, uint8_t offset_bit)
{
    *dst = 0;
    for (uint8_t i = 0; i < total_bit; i++)
    {
        *dst |= (((*src & (1 << (i + offset_bit))) >> i + offset_bit) << i);
    }
}
