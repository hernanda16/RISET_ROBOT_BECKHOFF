#include "boost/thread/mutex.hpp"
#include "iris_msgs/bs2pc.h"
#include "iris_msgs/data_robots.h"
#include "ros/ros.h"
#include "sensor_msgs/PointCloud.h"
#include "std_msgs/Float32MultiArray.h"
#include "std_msgs/UInt8.h"
#include "world_model/enums.hpp"
#include "world_model/utils.hpp"
#include <cv_bridge/cv_bridge.h>
#include <geometry_msgs/Pose2D.h>
#include <image_transport/image_transport.h>
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
    uint8_t n_has_ball; // Who has ball?
    uint8_t n_pass; // Who is passing?
    uint8_t n_pass_target; // Who is receiver of passing?

    uint8_t n_robot_att; // Number robot of attacker role
    uint8_t n_robot_assist; // Number robot of assist role
    uint8_t n_robot_gk; // Number robot of Goalkeeper role
    uint8_t n_robot_def_1; // Number robot of Defender1 role
    uint8_t n_robot_def_2; // Number robot of Defender2 role
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

using namespace cv;
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

image_transport::Publisher pub_layer_1;
image_transport::Publisher pub_layer_2;
image_transport::Publisher pub_layer_3;
image_transport::Publisher pub_layer_total;

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
std::vector<point_t> obs_merged;
std::vector<point_t> obs_pcl;

boost::mutex mutex_obs_merged;
boost::mutex mutex_obs_pcl;

uint8_t ball_status_buffer = 0;

Mat layer_total = Mat::zeros(Size(800, 1200), CV_8UC1);
Mat layer_1_raw = Mat::zeros(Size(800, 1200), CV_8UC1);
Mat layer_2_raw = Mat::zeros(Size(800, 1200), CV_8UC1);
Mat layer_3_raw = Mat::zeros(Size(800, 1200), CV_8UC1);
Mat layer_4_raw = Mat::zeros(Size(800, 1200), CV_8UC1);
Mat layer_1_gradient = Mat::zeros(Size(800, 1200), CV_8UC1);
Mat layer_2_gradient = Mat::zeros(Size(800, 1200), CV_8UC1);
Mat layer_3_gradient = Mat::zeros(Size(800, 1200), CV_8UC1);
Mat layer_4_gradient = Mat::zeros(Size(800, 1200), CV_8UC1);

//=========================

float titik_final[2];

//==========================

void callback_update(const ros::TimerEvent&);
void callback_sub_obs_merged(const sensor_msgs::PointCloudConstPtr& msg);
void callback_sub_bs_data(const iris_msgs::bs2pcConstPtr& msg);
void callback_sub_shared_data(const iris_msgs::data_robotsConstPtr& msg);
void callback_sub_final_ball(const geometry_msgs::Pose2DConstPtr& msg);
void callback_sub_ball_status(const std_msgs::UInt8ConstPtr& msg);

void readU16Bit(uint8_t* dst, const uint16_t* src, uint8_t total_bit, uint8_t offset_bit);
void make_layer_1();
void make_layer_2_dan_3();
void make_layer_2_dan_3_away();
void make_layer_4();
void make_layer_3(float obs_x, float obs_y);
void make_tangentCircle_polygon(float x1, float y1, float x2, float y2, float radius = 10, uint8_t full = 0, uint8_t layer_target = 3);
void make_polygon(Mat& _frame, vector<Point>& _pts, uint8_t val);
vector<float> tangentCircle(const Point2f& mouse, const Point2f& center, double radius);
Point get_min_value(Mat& _frame);

int main(int argc, char** argv)
{
    ros::init(argc, argv, "positioning");
    ros::NodeHandle NH;
    ros::MultiThreadedSpinner spinner(0);
    image_transport::ImageTransport IT(NH);

    //=================== PREPARE

    robot_saya_num = atoi(getenv("ROBOT"));
    memset(shared_robot_data, 0, sizeof(shared_robot_data));

    layer_1_raw = Scalar(0);
    layer_2_raw = Scalar(0);
    layer_3_raw = Scalar(0);
    layer_4_raw = Scalar(0);

    layer_1_gradient = Scalar(0);
    layer_2_gradient = Scalar(0);
    layer_3_gradient = Scalar(0);
    layer_4_gradient = Scalar(0);

    //==================================

    // tim_update = NH.createTimer(ros::Duration(0.5), callback_update);
    sub_obs = NH.subscribe("/world_model/obs/shared_all", 1, callback_sub_obs_merged);
    sub_shared_data = NH.subscribe("/communication/icast/data_robot", 1, callback_sub_shared_data);
    sub_bs2pc = NH.subscribe("/communication/icast/bs2pc", 1, callback_sub_bs_data);
    sub_ball_pos = NH.subscribe("/world_model/ball/final_pos", 1, callback_sub_final_ball);
    sub_ball_status = NH.subscribe("/world_model/ball/status", 1, callback_sub_ball_status);

    pub_layer_1 = IT.advertise("/world_model/positioning/field_layer_1", 1);
    pub_layer_2 = IT.advertise("/world_model/positioning/field_layer_2", 1);
    pub_layer_3 = IT.advertise("/world_model/positioning/field_layer_3", 1);
    pub_layer_total = IT.advertise("/world_model/positioning/field_layer_total", 1);

    pub_titik_final = NH.advertise<std_msgs::Float32MultiArray>("/world_model/positioning/titik_final_positioning", 1);

    // Hardcode ================================
    // shared_robot_data[2].pose.x = 400;
    // shared_robot_data[2].pose.y = 900;
    // shared_robot_data[3].pose.x = 600;
    // shared_robot_data[3].pose.y = 200;

    // ball.status = 2;
    // ball.x = 400;
    // ball.y = 600;

    // bs_data.n_robot_def_1 = 2;
    // bs_data.n_robot_att = 3;

    // bs_data.n_active_robot = 2;

    // point_t temp;
    // temp.x = 600;
    // temp.y = 800;
    // obs_merged.push_back(temp);

    // point_t temp2;
    // temp2.x = 600;
    // temp2.y = 800;
    // obs_merged.push_back(temp2);

    // robot_saya_role = 1;
    // END OF HARDCODE

    spinner.spin();
    tim_update.stop();

    return 0;
}

//==========================================

void callback_sub_ball_status(const std_msgs::UInt8ConstPtr& msg)
{
    ball.status = msg->data;
    ball_status_buffer = ball.status;
    // printf("CLLBCL: %d\n", ball.status);
}

void callback_sub_final_ball(const geometry_msgs::Pose2DConstPtr& msg)
{
    ball.x = msg->x;
    ball.y = msg->y;
}

void callback_sub_obs_merged(const sensor_msgs::PointCloudConstPtr& msg)
{
    // mutex_obs_merged.lock();
    obs_merged.clear();
    for (int i = 0; i < msg->points.size(); i++) {
        point_t temp;
        temp.x = msg->points[i].x;
        temp.y = msg->points[i].y;
        obs_merged.push_back(temp);
    }
    // mutex_obs_merged.unlock();

    float min_distance = FLT_MAX;
    for (int i = 0; i < obs_merged.size(); i++) {
        if (obs_merged[i].y < pos_robot_y && obs_merged[i].y > teman_y) {
            if (pythagoras(pos_robot_x, pos_robot_y, obs_merged[i].x, obs_merged[i].y) < min_distance) {
                min_distance = pythagoras(pos_robot_x, pos_robot_y, obs_merged[i].x, obs_merged[i].y);
                obs_terdekat.x = obs_merged[i].x;
                obs_terdekat.y = obs_merged[i].y;
                obs_terdekat.distance = min_distance;
            }
        }
    }

    static uint8_t counter = 0;

    if (counter++ <= 25) {
        return;
    }
    counter = 0;

// KEYBOARD UNTUK SIMULASI
#ifndef KEYBOARD
// #define KEYBOARD
#endif
#ifdef KEYBOARD
    if (kbhit() > 0) {
        char c = std::getchar();

        switch (c) {
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

    layer_total = Scalar(0);
    layer_1_raw = Scalar(0);
    layer_2_raw = Scalar(0);
    layer_3_raw = Scalar(0);
    layer_4_raw = Scalar(0);

    layer_1_gradient = Scalar(0);
    layer_2_gradient = Scalar(0);
    layer_3_gradient = Scalar(0);
    layer_4_gradient = Scalar(0);

    make_layer_1();

    switch (game_status) {
    case status_preparation_kickoff_away:
    case status_preparation_freekick_away:
    case status_preparation_goalkick_away:
    case status_preparation_throwin_away:
    case status_preparation_cornerkick_away:
        make_layer_2_dan_3_away();
        break;

    default:
        make_layer_2_dan_3();
        // make_layer_4();
        break;
    }

    layer_total = layer_1_gradient + layer_2_gradient + layer_3_gradient + layer_4_gradient;

    Point titik_final_pt = get_min_value(layer_total);
    titik_final[0] = titik_final_pt.x;
    titik_final[1] = titik_final_pt.y;

    Mat colormap;
    applyColorMap(layer_total, colormap, COLORMAP_JET);

    circle(colormap, Point(shared_robot_data[robot_saya_num].pose.x, shared_robot_data[robot_saya_num].pose.y), 25, Scalar(0, 0, 255), 3);
    circle(colormap, Point(teman_x, teman_y), 30, Scalar(0, 255, 0), -1);
    circle(colormap, titik_final_pt, 15, Scalar(0, 0, 255), -1);

    // ===================== RESIZE
    Mat display_layer_1, display_layer_2, display_layer_3, display_layer_total;
    resize(layer_1_gradient, display_layer_1, Size(400, 600));
    flip(display_layer_1, display_layer_1, 0);
    resize(layer_4_raw, display_layer_2, Size(400, 600));
    flip(display_layer_2, display_layer_2, 0);
    resize(layer_3_gradient, display_layer_3, Size(400, 600));
    flip(display_layer_3, display_layer_3, 0);
    resize(colormap, display_layer_total, Size(400, 600));
    flip(display_layer_total, display_layer_total, 0);

    sensor_msgs::ImagePtr msg_layer_1 = cv_bridge::CvImage(std_msgs::Header(), "mono8", display_layer_1).toImageMsg();
    pub_layer_1.publish(msg_layer_1);
    sensor_msgs::ImagePtr msg_layer_2 = cv_bridge::CvImage(std_msgs::Header(), "mono8", display_layer_2).toImageMsg();
    pub_layer_2.publish(msg_layer_2);
    sensor_msgs::ImagePtr msg_layer_3 = cv_bridge::CvImage(std_msgs::Header(), "mono8", display_layer_3).toImageMsg();
    pub_layer_3.publish(msg_layer_3);
    sensor_msgs::ImagePtr msg_layer_total = cv_bridge::CvImage(std_msgs::Header(), "bgr8", display_layer_total).toImageMsg();
    pub_layer_total.publish(msg_layer_total);

    std_msgs::Float32MultiArray msg_titik_final;
    msg_titik_final.data.push_back(titik_final[0]);
    msg_titik_final.data.push_back(titik_final[1]);
    pub_titik_final.publish(msg_titik_final);
}

void callback_sub_bs_data(const iris_msgs::bs2pcConstPtr& msg)
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

    for (uint8_t i = 0; i < 4; i++) {
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
            bs_data.n_robot_gk = i;
            break;
        }
    }

    robot_saya_role = shared_robot_data[robot_num].role;

    // Proses Command BS
    static uint8_t prev_BS_cmd = 0;

    static const uint8_t robot_num_bin[6] = { 0b00000, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000 };

    bs_data.is_iam_controlled_by_bs = (msg->data_mux_control & (robot_num_bin[robot_num])) >> (robot_num - 1);

    if (bs_data.is_iam_controlled_by_bs) {
        if (prev_BS_cmd != bs_data.command) {
            if (bs_data.command == 's') {
                if (game_status > 0 && game_status <= 127)
                    game_status += 128;
            } else if (bs_data.command == 'S') {
                if (game_status > 127 && game_status <= 255)
                    game_status -= 128;
            } else if (bs_data.command != 126) {
                game_status = bs_data.command;
            }

            // robot_base_action = (!(prev_BS_cmd == 126 && bs_data.command == 'S') && bs_data.command != ' ') * (bs_data.command != 126) | (bs_data.command == status_park);
        }
        prev_BS_cmd = bs_data.command;
    } else {
        prev_BS_cmd = 126;
    }
}
void callback_sub_shared_data(const iris_msgs::data_robotsConstPtr& msg)
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
}

//==================================================

void callback_update(const ros::TimerEvent&)
{
    // KEYBOARD UNTUK SIMULASI
// #define KEYBOARD
#ifdef KEYBOARD
    if (kbhit() > 0) {
        char c = std::getchar();

        switch (c) {
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

        shared_robot_data[2].pose.x = 200;
        shared_robot_data[2].pose.y = 200;
        shared_robot_data[3].pose.x = 600;
        shared_robot_data[3].pose.y = 800;
    }
#endif
    //========================

    layer_total = Scalar(0);
    layer_1_raw = Scalar(0);
    layer_2_raw = Scalar(0);
    layer_3_raw = Scalar(0);
    layer_1_gradient = Scalar(0);
    layer_2_gradient = Scalar(0);
    layer_3_gradient = Scalar(0);

    make_layer_1();

    switch (game_status) {
    case status_preparation_kickoff_away:
    case status_preparation_freekick_away:
    case status_preparation_goalkick_away:
    case status_preparation_throwin_away:
    case status_preparation_cornerkick_away:
        make_layer_2_dan_3_away();
        break;

    default:
        make_layer_2_dan_3();
        break;
    }

    layer_total = layer_1_gradient + layer_2_gradient + layer_3_gradient;

    Point titik_final_pt = get_min_value(layer_total);
    titik_final[0] = titik_final_pt.x;
    titik_final[1] = titik_final_pt.y;

    Mat colormap;
    applyColorMap(layer_total, colormap, COLORMAP_JET);

    circle(colormap, Point(shared_robot_data[robot_saya_num].pose.x, shared_robot_data[robot_saya_num].pose.y), 25, Scalar(0, 0, 255), 3);
    circle(colormap, Point(teman_x, teman_y), 30, Scalar(0, 255, 0), -1);
    circle(colormap, titik_final_pt, 15, Scalar(0, 0, 255), -1);

    // ===================== RESIZE
    Mat display_layer_1, display_layer_2, display_layer_3, display_layer_total;
    resize(layer_1_gradient, display_layer_1, Size(400, 600));
    flip(display_layer_1, display_layer_1, 0);
    resize(layer_2_gradient, display_layer_2, Size(400, 600));
    flip(display_layer_2, display_layer_2, 0);
    resize(layer_3_gradient, display_layer_3, Size(400, 600));
    flip(display_layer_3, display_layer_3, 0);
    resize(colormap, display_layer_total, Size(400, 600));
    flip(display_layer_total, display_layer_total, 0);

    sensor_msgs::ImagePtr msg_layer_1 = cv_bridge::CvImage(std_msgs::Header(), "mono8", display_layer_1).toImageMsg();
    pub_layer_1.publish(msg_layer_1);
    sensor_msgs::ImagePtr msg_layer_2 = cv_bridge::CvImage(std_msgs::Header(), "mono8", display_layer_2).toImageMsg();
    pub_layer_2.publish(msg_layer_2);
    sensor_msgs::ImagePtr msg_layer_3 = cv_bridge::CvImage(std_msgs::Header(), "mono8", display_layer_3).toImageMsg();
    pub_layer_3.publish(msg_layer_3);
    sensor_msgs::ImagePtr msg_layer_total = cv_bridge::CvImage(std_msgs::Header(), "bgr8", display_layer_total).toImageMsg();
    pub_layer_total.publish(msg_layer_total);

    std_msgs::Float32MultiArray msg_titik_final;
    msg_titik_final.data.push_back(titik_final[0]);
    msg_titik_final.data.push_back(titik_final[1]);
    pub_titik_final.publish(msg_titik_final);
}

//===================================================

void make_layer_1()
{
    static const uint8_t max_value = 127;
    static const uint8_t max_distance_layer_1 = 5;

    static uint16_t radius_kickoff_away = 130; // rule radius > 200
    static uint16_t radius_freekick_home = 200; // rule radius > 200
    static uint16_t radius_freekick_away = 150; // rule radius > 300
    static uint16_t radius_throwin_home = 200; // rule radius > ?
    static uint16_t radius_throwin_away = 200; // rule radius > ?
    static uint16_t radius_goalkick_home = 220; // rule radius > 200
    static uint16_t radius_goalkick_away = 230; // rule radius > 300
    // static uint16_t radius_corner_home = 200; // rule radius > 200
    // static uint16_t radius_corner_away = 250; // rule radius > 300
    static uint16_t radius_drop_ball = 100;

    if (robot_saya_role == 1) {
        teman_x = shared_robot_data[bs_data.n_robot_def_1].pose.x;
        teman_y = shared_robot_data[bs_data.n_robot_def_1].pose.y;
        pos_robot_x = shared_robot_data[bs_data.n_robot_att].pose.x;
        pos_robot_y = shared_robot_data[bs_data.n_robot_att].pose.y;
    } else {
        teman_x = shared_robot_data[bs_data.n_robot_att].pose.x;
        teman_y = shared_robot_data[bs_data.n_robot_att].pose.y;
        pos_robot_x = shared_robot_data[bs_data.n_robot_def_1].pose.x;
        pos_robot_y = shared_robot_data[bs_data.n_robot_def_1].pose.y;
    }

    // ---------------------------------
    //              TEMPLATE
    // ---------------------------------

    //=== Cover Side
    {
        vector<Point> pts;
        pts.push_back(Point(0, 0));
        pts.push_back(Point(30, 0));
        pts.push_back(Point(30, 1200));
        pts.push_back(Point(0, 1200));
        make_polygon(layer_1_raw, pts, 255);
    }
    {
        vector<Point> pts;
        pts.push_back(Point(800, 0));
        pts.push_back(Point(770, 0));
        pts.push_back(Point(770, 1200));
        pts.push_back(Point(800, 1200));
        make_polygon(layer_1_raw, pts, 255);
    }

    //=== Cover Corner
    {
        vector<Point> pts;
        pts.push_back(Point(0, 1200));
        pts.push_back(Point(300, 1200));
        pts.push_back(Point(0, 900));
        make_polygon(layer_1_raw, pts, 255);
    }
    {
        vector<Point> pts;
        pts.push_back(Point(800, 1200));
        pts.push_back(Point(500, 1200));
        pts.push_back(Point(800, 900));
        make_polygon(layer_1_raw, pts, 255);
    }
    {
        vector<Point> pts;
        pts.push_back(Point(0, 0));
        pts.push_back(Point(300, 0));
        pts.push_back(Point(0, 300));
        make_polygon(layer_1_raw, pts, 255);
    }
    {
        vector<Point> pts;
        pts.push_back(Point(800, 0));
        pts.push_back(Point(500, 0));
        pts.push_back(Point(800, 300));
        make_polygon(layer_1_raw, pts, 255);
    }

    //=== Cover Penalty
    {
        vector<Point> pts;
        pts.push_back(Point(150, 150));
        pts.push_back(Point(650, 150));
        pts.push_back(Point(650, 0));
        pts.push_back(Point(150, 0));
        make_polygon(layer_1_raw, pts, 255);
    }
    {
        vector<Point> pts;
        pts.push_back(Point(150, 1050));
        pts.push_back(Point(650, 1050));
        pts.push_back(Point(650, 1200));
        pts.push_back(Point(150, 1200));
        make_polygon(layer_1_raw, pts, 255);
    }

    // ROS_ERROR("GAME STATUS: %d", game_status);
    switch (game_status) {
    case status_preparation_kickoff_away:

        // ---------------------------------
        //           KICK OFF AWAY
        // ---------------------------------

        //=== Threshold Rule Radius
        {
            if (ball.status == 0) {
                circle(layer_1_raw, Point(FIELD_X_1_2, FIELD_Y_1_2), radius_kickoff_away, Scalar(255), -1);
            } else {
                circle(layer_1_raw, Point(ball.x, FIELD_Y_1_2), radius_kickoff_away, Scalar(255), -1);
            }
        }

        //=== Cover Away
        {
            vector<Point> pts;
            pts.push_back(Point(0, 1200));
            pts.push_back(Point(0, FIELD_Y_1_2));
            pts.push_back(Point(800, FIELD_Y_1_2));
            pts.push_back(Point(800, 1200));
            make_polygon(layer_1_raw, pts, 255);
        }

        // AREA ROBOT BELAKANG
        if (robot_saya_role == 2) {
            vector<Point> pts;
            pts.push_back(Point(0, 600));
            pts.push_back(Point(800, 600));
            pts.push_back(Point(800, 300));
            pts.push_back(Point(0, 300));
            make_polygon(layer_1_raw, pts, 255);
        }

        // AREA ROBOT DEPAN
        else if (robot_saya_role == 1) {
            vector<Point> pts;
            pts.push_back(Point(0, 270));
            pts.push_back(Point(800, 270));
            pts.push_back(Point(800, 0));
            pts.push_back(Point(0, 0));
            make_polygon(layer_1_raw, pts, 255);
        }

        break;

    case status_preparation_freekick_home:
        // ---------------------------------
        //          FREE KICK HOME
        // ---------------------------------
        //=== Threshold Rule Radius
        {
            if (ball.status == 0) {
                circle(layer_1_raw, Point(FIELD_X_1_2, FIELD_Y_1_2), radius_freekick_home, Scalar(255), -1);
            } else {
                circle(layer_1_raw, Point(ball.x, FIELD_Y_1_2), radius_freekick_home, Scalar(255), -1);
            }
        }

        //=== Cover Home
        {
            vector<Point> pts;
            pts.push_back(Point(0, 0));
            pts.push_back(Point(0, FIELD_Y_2_3));
            pts.push_back(Point(800, FIELD_Y_2_3));
            pts.push_back(Point(800, 0));
            make_polygon(layer_1_raw, pts, 255);
        }

        // printf("BALL: %.2f OBS: %.2f DIST: %.2f\n", ball.x, obs_terdekat.x, obs_terdekat.distance);

        if (ball.x > FIELD_X_1_2) {
            // if (obs_terdekat.distance > 200) {
            // printf("LOGGER : 1\n");
            vector<Point> pts;
            pts.push_back(Point(0, 0));
            pts.push_back(Point(FIELD_X_1_2, 0));
            pts.push_back(Point(FIELD_X_1_2, 1200));
            pts.push_back(Point(0, 1200));
            make_polygon(layer_1_raw, pts, 255);
            // }
        } else if (ball.x < FIELD_X_1_2) {
            // if (obs_terdekat.distance > 200) {
            // printf("LOGGER : 2\n");
            vector<Point> pts;
            pts.push_back(Point(FIELD_X_1, 0));
            pts.push_back(Point(FIELD_X_2_3, 0));
            pts.push_back(Point(FIELD_X_2_3, 1200));
            pts.push_back(Point(FIELD_X_1, 1200));
            make_polygon(layer_1_raw, pts, 255);
            // }
        }
        //  else {
        //     if (obs_terdekat.x < pos_robot_x) {
        //         if (obs_terdekat.distance > 200) {
        //             printf("LOGGER : 3\n");
        //             vector<Point> pts;
        //             pts.push_back(Point(0, 0));
        //             pts.push_back(Point(FIELD_X_2_3, 0));
        //             pts.push_back(Point(FIELD_X_2_3, 1200));
        //             pts.push_back(Point(0, 1200));
        //             make_polygon(layer_1_raw, pts, 255);
        //         }
        //     } else {
        //         if (obs_terdekat.distance > 200) {
        //             printf("LOGGER : 4\n");
        //             vector<Point> pts;
        //             pts.push_back(Point(FIELD_X_1, 0));
        //             pts.push_back(Point(FIELD_X_1_3, 0));
        //             pts.push_back(Point(FIELD_X_1_3, 1200));
        //             pts.push_back(Point(FIELD_X_1, 1200));
        //             make_polygon(layer_1_raw, pts, 255);
        //         }
        //     }
        // }

        break;

    case status_preparation_freekick_away:
        // ---------------------------------
        //          FREE KICK AWAY
        // ---------------------------------

        //=== Threshold Rule Radius
        {
            if (ball.status == 0) {
                circle(layer_1_raw, Point(FIELD_X_1_2, FIELD_Y_1_2), radius_freekick_away, Scalar(255), -1);
            } else {
                circle(layer_1_raw, Point(ball.x, FIELD_Y_1_2), radius_freekick_away, Scalar(255), -1);
            }
        }

        //=== Cover Away
        {
            vector<Point> pts;
            pts.push_back(Point(0, 1200));
            pts.push_back(Point(0, FIELD_Y_1_2));
            pts.push_back(Point(800, FIELD_Y_1_2));
            pts.push_back(Point(800, 1200));
            make_polygon(layer_1_raw, pts, 255);
        }

        // AREA ROBOT BELAKANG
        if (robot_saya_role == 2) {
            vector<Point> pts;
            pts.push_back(Point(0, 600));
            pts.push_back(Point(800, 600));
            pts.push_back(Point(800, 300));
            pts.push_back(Point(0, 300));
            make_polygon(layer_1_raw, pts, 255);
        }

        // AREA ROBOT DEPAN
        else if (robot_saya_role == 1) {
            vector<Point> pts;
            pts.push_back(Point(0, 250));
            pts.push_back(Point(800, 250));
            pts.push_back(Point(800, 0));
            pts.push_back(Point(0, 0));
            make_polygon(layer_1_raw, pts, 255);
        }

        break;

    case status_preparation_throwin_home:
        // ---------------------------------
        //           THROWIN HOME
        // ---------------------------------

        {
            vector<Point> pts;
            pts.push_back(Point(0, FIELD_Y_1_2 - 100));
            pts.push_back(Point(800, FIELD_Y_1_2 - 100));
            pts.push_back(Point(800, 0));
            pts.push_back(Point(0, 0));
            make_polygon(layer_1_raw, pts, 255);
        }

        if (ball.status > 0) {
            //=== Threshold Rule Radius
            circle(layer_1_raw, Point(ball.x, ball.y), radius_throwin_home, Scalar(255), -1);

            {
                vector<Point> pts;
                pts.push_back(Point(teman_x < FIELD_X_1_2 ? FIELD_X_0 : FIELD_X_2_3, FIELD_Y_0));
                pts.push_back(Point(teman_x < FIELD_X_1_2 ? FIELD_X_1_3 : FIELD_X_1, FIELD_Y_0));
                pts.push_back(Point(teman_x < FIELD_X_1_2 ? FIELD_X_1_3 : FIELD_X_1, FIELD_Y_1));
                pts.push_back(Point(teman_x < FIELD_X_1_2 ? FIELD_X_0 : FIELD_X_2_3, FIELD_Y_1));
                make_polygon(layer_1_raw, pts, 255);
            }
        } else {
            vector<Point> pts;
            pts.push_back(Point(robot_saya_role == 1 ? FIELD_X_0 : FIELD_X_1_2, FIELD_Y_0));
            pts.push_back(Point(robot_saya_role == 1 ? FIELD_X_1_2 : FIELD_X_1, FIELD_Y_0));
            pts.push_back(Point(robot_saya_role == 1 ? FIELD_X_1_2 : FIELD_X_1, FIELD_Y_1));
            pts.push_back(Point(robot_saya_role == 1 ? FIELD_X_0 : FIELD_X_1_2, FIELD_Y_1));
            make_polygon(layer_1_raw, pts, 255);
        }

        break;

    case status_preparation_throwin_away:
        // ---------------------------------
        //           THROWIN AWAY
        // ---------------------------------

        if (ball.status > 0) {
            //=== Threshold Rule Radius
            circle(layer_1_raw, Point(ball.x, ball.y), radius_throwin_away, Scalar(255), -1);

            // AREA ROBOT BELAKANG
            if (robot_saya_role == 2) {
                vector<Point> pts;
                pts.push_back(Point(0, FIELD_Y_1));
                pts.push_back(Point(800, FIELD_Y_1));
                pts.push_back(Point(800, 300));
                pts.push_back(Point(0, 300));
                make_polygon(layer_1_raw, pts, 255);
            }

            // AREA ROBOT DEPAN
            else if (robot_saya_role == 1) {
                vector<Point> pts;
                pts.push_back(Point(0, 250));
                pts.push_back(Point(800, 250));
                pts.push_back(Point(800, 0));
                pts.push_back(Point(0, 0));
                make_polygon(layer_1_raw, pts, 255);
            }
        } else {
            vector<Point> pts;
            pts.push_back(Point(robot_saya_role == 1 ? FIELD_X_0 : FIELD_X_1_2, FIELD_Y_0));
            pts.push_back(Point(robot_saya_role == 1 ? FIELD_X_1_2 : FIELD_X_1, FIELD_Y_0));
            pts.push_back(Point(robot_saya_role == 1 ? FIELD_X_1_2 : FIELD_X_1, FIELD_Y_1));
            pts.push_back(Point(robot_saya_role == 1 ? FIELD_X_0 : FIELD_X_1_2, FIELD_Y_1));
            make_polygon(layer_1_raw, pts, 255);
        }

        break;

    case status_preparation_goalkick_home:
        // ---------------------------------
        //          GOAL KICK HOME
        // ---------------------------------

        //=== Threshold Rule Radius
        {
            if (ball.status == 0) {
                circle(layer_1_raw, Point(FIELD_X_1_2, FIELD_Y_1_6), radius_goalkick_home, Scalar(255), -1);
            } else {
                circle(layer_1_raw, Point(ball.x, FIELD_Y_1_6), radius_goalkick_home, Scalar(255), -1);
            }
        }

        //=== Cover Home
        {
            vector<Point> pts;
            pts.push_back(Point(0, 1020));
            pts.push_back(Point(0, FIELD_Y_1));
            pts.push_back(Point(800, FIELD_Y_1));
            pts.push_back(Point(800, 1020));
            make_polygon(layer_1_raw, pts, 255);
        }

        {
            vector<Point> pts;
            pts.push_back(Point(0, 0));
            pts.push_back(Point(0, FIELD_Y_1_2 + 200));
            pts.push_back(Point(800, FIELD_Y_1_2 + 200));
            pts.push_back(Point(800, 0));
            make_polygon(layer_1_raw, pts, 255);
        }

        // printf("BALL: %.2f OBS: %.2f DIST: %.2f\n", ball.x, obs_terdekat.x, obs_terdekat.distance);

        if (teman_x >= FIELD_X_1_2) {
            // if (obs_terdekat.distance > 200) {
            // printf("LOGGER : 1\n");
            vector<Point> pts;
            pts.push_back(Point(0, 0));
            pts.push_back(Point(FIELD_X_1_2, 0));
            pts.push_back(Point(FIELD_X_1_2, 1200));
            pts.push_back(Point(0, 1200));
            make_polygon(layer_1_raw, pts, 255);
            // }
        } else if (teman_x < FIELD_X_1_2) {
            // if (obs_terdekat.distance > 200) {
            // printf("LOGGER : 2\n");
            vector<Point> pts;
            pts.push_back(Point(FIELD_X_1, 0));
            pts.push_back(Point(FIELD_X_1_2, 0));
            pts.push_back(Point(FIELD_X_1_2, 1200));
            pts.push_back(Point(FIELD_X_1, 1200));
            make_polygon(layer_1_raw, pts, 255);
            // }
        }
        // else {
        //     if (obs_terdekat.x < pos_robot_x) {
        //         if (obs_terdekat.distance > 200) {
        //             printf("LOGGER : 3\n");
        //             vector<Point> pts;
        //             pts.push_back(Point(0, 0));
        //             pts.push_back(Point(FIELD_X_2_3, 0));
        //             pts.push_back(Point(FIELD_X_2_3, 1200));
        //             pts.push_back(Point(0, 1200));
        //             make_polygon(layer_1_raw, pts, 255);
        //         }
        //     } else {
        //         if (obs_terdekat.distance > 200) {
        //             printf("LOGGER : 4\n");
        //             vector<Point> pts;
        //             pts.push_back(Point(FIELD_X_1, 0));
        //             pts.push_back(Point(FIELD_X_1_3, 0));
        //             pts.push_back(Point(FIELD_X_1_3, 1200));
        //             pts.push_back(Point(FIELD_X_1, 1200));
        //             make_polygon(layer_1_raw, pts, 255);
        //         }
        //     }
        // }

        break;

    case status_preparation_goalkick_away:
        // ---------------------------------
        //          GOAL KICK AWAY
        // ---------------------------------

        //=== Threshold Rule Radius
        {
            if (ball.status == 0) {
                circle(layer_1_raw, Point(FIELD_X_1_2, FIELD_Y_5_6), radius_goalkick_away, Scalar(255), -1);
            } else {
                circle(layer_1_raw, Point(ball.x, FIELD_Y_5_6), radius_goalkick_away, Scalar(255), -1);
            }
        }

        //=== Cover Away
        {
            vector<Point> pts;
            pts.push_back(Point(0, 1200));
            pts.push_back(Point(0, FIELD_Y_5_6));
            pts.push_back(Point(800, FIELD_Y_5_6));
            pts.push_back(Point(800, 1200));
            make_polygon(layer_1_raw, pts, 255);
        }

        // AREA ROBOT BELAKANG
        if (robot_saya_role == 2) {
            vector<Point> pts;
            pts.push_back(Point(0, FIELD_Y_5_6));
            pts.push_back(Point(800, FIELD_Y_5_6));
            pts.push_back(Point(800, 350));
            pts.push_back(Point(0, 350));
            make_polygon(layer_1_raw, pts, 255);
        }

        // AREA ROBOT DEPAN
        else if (robot_saya_role == 1) {
            vector<Point> pts;
            pts.push_back(Point(0, 350));
            pts.push_back(Point(800, 350));
            pts.push_back(Point(800, 0));
            pts.push_back(Point(0, 0));
            make_polygon(layer_1_raw, pts, 255);
        }

        break;

    case status_preparation_cornerkick_home:
        // ---------------------------------
        //          CORNER KICK HOME
        // ---------------------------------

        if (ball.status > 0) {
            {
                vector<Point> pts;
                pts.push_back(Point(teman_x < FIELD_X_1_2 ? FIELD_X_0 : FIELD_X_1_2, FIELD_Y_1_2));
                pts.push_back(Point(teman_x < FIELD_X_1_2 ? FIELD_X_1_2 : FIELD_X_1, FIELD_Y_1_2));
                pts.push_back(Point(teman_x < FIELD_X_1_2 ? FIELD_X_1_2 : FIELD_X_1, FIELD_Y_1));
                pts.push_back(Point(teman_x < FIELD_X_1_2 ? FIELD_X_0 : FIELD_X_1_2, FIELD_Y_1));
                make_polygon(layer_1_raw, pts, 255);
            }

            //=== Cover Home
            {
                vector<Point> pts;
                pts.push_back(Point(FIELD_X_0, FIELD_Y_0));
                pts.push_back(Point(FIELD_X_1, FIELD_Y_0));
                pts.push_back(Point(FIELD_X_1, FIELD_Y_1_2));
                pts.push_back(Point(FIELD_X_0, FIELD_Y_1_2));
                make_polygon(layer_1_raw, pts, 255);
            }
        } else {
            {
                vector<Point> pts;
                pts.push_back(Point(robot_saya_role == 1 ? FIELD_X_0 : FIELD_X_1_2, 0));
                pts.push_back(Point(robot_saya_role == 1 ? FIELD_X_1_2 : FIELD_X_1, 0));
                pts.push_back(Point(robot_saya_role == 1 ? FIELD_X_1_2 : FIELD_X_1, FIELD_Y_1));
                pts.push_back(Point(robot_saya_role == 1 ? FIELD_X_0 : FIELD_X_1_2, FIELD_Y_1));
                make_polygon(layer_1_raw, pts, 255);
            }

            //=== Cover Home
            {
                vector<Point> pts;
                pts.push_back(Point(FIELD_X_0, FIELD_Y_0));
                pts.push_back(Point(FIELD_X_1, FIELD_Y_0));
                pts.push_back(Point(FIELD_X_1, FIELD_Y_1_2));
                pts.push_back(Point(FIELD_X_0, FIELD_Y_1_2));
                make_polygon(layer_1_raw, pts, 255);
            }
        }

        break;
    case status_preparation_cornerkick_away:
        // ---------------------------------
        //          CORNER KICK AWAY
        // ---------------------------------

        // //=== Cover Away
        {
            vector<Point> pts;
            pts.push_back(Point(FIELD_X_0, FIELD_Y_1));
            pts.push_back(Point(FIELD_X_1, FIELD_Y_1));
            pts.push_back(Point(FIELD_X_1, FIELD_Y_1_3));
            pts.push_back(Point(FIELD_X_0, FIELD_Y_1_3));
            make_polygon(layer_1_raw, pts, 255);
        }

        // printf("HALO: %d\n", ball.status);
        if (ball.status > 0) {
            // AREA ROBOT DEPAN
            if ((teman_x - FIELD_X_1_2) < (pos_robot_x - FIELD_X_1_2)) {
                {
                    vector<Point> pts;
                    pts.push_back(Point(ball.x > FIELD_X_1_2 ? FIELD_X_0 : FIELD_X_1_2, 0));
                    pts.push_back(Point(ball.x > FIELD_X_1_2 ? FIELD_X_1_2 : FIELD_X_1, 0));
                    pts.push_back(Point(ball.x > FIELD_X_1_2 ? FIELD_X_1_2 : FIELD_X_1, FIELD_Y_1));
                    pts.push_back(Point(ball.x > FIELD_X_1_2 ? FIELD_X_0 : FIELD_X_1_2, FIELD_Y_1));
                    make_polygon(layer_1_raw, pts, 255);
                }
            }

            // AREA ROBOT BELAKANG
            else {
                {
                    vector<Point> pts;
                    pts.push_back(Point(ball.x < FIELD_X_1_2 ? FIELD_X_0 : FIELD_X_1_2, 0));
                    pts.push_back(Point(ball.x < FIELD_X_1_2 ? FIELD_X_1_2 : FIELD_X_1, 0));
                    pts.push_back(Point(ball.x < FIELD_X_1_2 ? FIELD_X_1_2 : FIELD_X_1, FIELD_Y_1));
                    pts.push_back(Point(ball.x < FIELD_X_1_2 ? FIELD_X_0 : FIELD_X_1_2, FIELD_Y_1));
                    make_polygon(layer_1_raw, pts, 255);
                }
            }
        } else {
            vector<Point> pts;
            pts.push_back(Point(FIELD_X_1_2, 0));
            pts.push_back(Point(FIELD_X_0, 0));
            pts.push_back(Point(FIELD_X_0, FIELD_Y_1));
            pts.push_back(Point(FIELD_X_1_2, FIELD_Y_1));
            make_polygon(layer_1_raw, pts, 255);

            if (teman_x < pos_robot_x) {
            } else {
                vector<Point> pts;
                pts.push_back(Point(FIELD_X_1, 0));
                pts.push_back(Point(FIELD_X_1_2, 0));
                pts.push_back(Point(FIELD_X_1_2, FIELD_Y_1));
                pts.push_back(Point(FIELD_X_1, FIELD_Y_1));
                make_polygon(layer_1_raw, pts, 255);
            }
        }

        break;

    case status_preparation_dropball:
        // ---------------------------------
        //            DROP BALL
        // ---------------------------------

        //=== Threshold Rule Radius
        circle(layer_1_raw, Point(ball.x, ball.y), radius_drop_ball, Scalar(255), -1);

        break;

    default:
        break;
    }

    // ---------------------------------

    Mat buffer = layer_1_raw.clone();
    bitwise_not(buffer, buffer);
    distanceTransform(buffer, buffer, DIST_L2, 3);
    threshold(buffer, buffer, max_distance_layer_1, 255, THRESH_TRUNC);
    normalize(buffer, buffer, 0, 255, NORM_MINMAX, CV_8UC1);
    bitwise_not(buffer, buffer);

    // ---------------------------------

    normalize(buffer, layer_1_gradient, 0, max_value, NORM_MINMAX, CV_8UC1);
}

void make_layer_2_dan_3()
{
    static const uint8_t max_value = 63;
    static const uint8_t max_distance_layer_2 = 100;
    static const int r_menjauhi_teman_layer_3 = 350;
    static const uint8_t max_value_layer_3 = 30;
    static const uint8_t max_distance_layer_3 = 70;

    // Ketika saya sebagai attacker, maka teman adalah def_1
    if (robot_saya_role == 1) {
        teman_x = shared_robot_data[bs_data.n_robot_def_1].pose.x;
        teman_y = shared_robot_data[bs_data.n_robot_def_1].pose.y;
    } else {
        teman_x = shared_robot_data[bs_data.n_robot_att].pose.x;
        teman_y = shared_robot_data[bs_data.n_robot_att].pose.y;
    }

    // printf("Hello: (%d) %d %d || %.2f %.2f %.2f %.2f -> %.2f %.2f\n", robot_saya_role, bs_data.n_robot_def_1, bs_data.n_robot_att,
    //        shared_robot_data[bs_data.n_robot_def_1].pose.x, shared_robot_data[bs_data.n_robot_def_1].pose.y,
    //        shared_robot_data[bs_data.n_robot_att].pose.x, shared_robot_data[bs_data.n_robot_att].pose.y,
    //        teman_x, teman_y);

    mutex_obs_merged.lock();
    for (int i = 0; i < obs_merged.size(); i++) {

        float obs_x = obs_merged[i].x;
        float obs_y = obs_merged[i].y;

        if (sqrtf((teman_x - obs_x) * (teman_x - obs_x) + (teman_y - obs_y) * (teman_y - obs_y)) < 100)
            continue;

        if (bs_data.n_active_robot > 0)
            make_layer_3(obs_x, obs_y);

        // if (game_status != status_preparation_goalkick_home && game_status != status_preparation_freekick_home)
        // make_tangentCircle_polygon(gawang_lawan_x, gawang_lawan_y, obs_x, obs_y, 15);
        circle(layer_2_raw, Point(obs_x, obs_y), 15, Scalar(255), -1);
    }

    mutex_obs_merged.unlock();

    if (bs_data.n_active_robot > 0)
        circle(layer_2_raw, Point(teman_x, teman_y), r_menjauhi_teman_layer_3, Scalar(255), -1);

    Mat buffer = layer_2_raw.clone();
    bitwise_not(buffer, buffer);
    distanceTransform(buffer, buffer, DIST_L2, 3);
    threshold(buffer, buffer, max_distance_layer_2, 255, THRESH_TRUNC);
    normalize(buffer, buffer, 0, 255, NORM_MINMAX, CV_8UC1);
    bitwise_not(buffer, buffer);
    normalize(buffer, layer_2_gradient, 0, max_value, NORM_MINMAX, CV_8UC1);

    // ---------------------------------

    //     DIST_USER    = -1,  //!< User defined distance
    //     DIST_L1      = 1,   //!< distance = |x1-x2| + |y1-y2|
    //     DIST_L2      = 2,   //!< the simple euclidean distance
    //     DIST_C       = 3,   //!< distance = max(|x1-x2|,|y1-y2|)

    Mat buffer_layer_3 = layer_3_raw.clone();
    bitwise_not(buffer_layer_3, buffer_layer_3);
    distanceTransform(buffer_layer_3, buffer_layer_3, DIST_L2, 5);
    threshold(buffer_layer_3, buffer_layer_3, max_distance_layer_3, 255, THRESH_TRUNC);
    normalize(buffer_layer_3, buffer_layer_3, 0, 255, NORM_MINMAX, CV_8UC1);
    bitwise_not(buffer_layer_3, buffer_layer_3);

    // ---------------------------------

    normalize(buffer_layer_3, layer_3_gradient, 0, max_value_layer_3, NORM_MINMAX, CV_8UC1);
}

void make_layer_3(float obs_x, float obs_y)
{
    make_tangentCircle_polygon(teman_x, teman_y, obs_x, obs_y, 25);
    make_tangentCircle_polygon(shared_robot_data[robot_saya_num].pose.x, shared_robot_data[robot_saya_num].pose.y, obs_x, obs_y);
}

void make_layer_2_dan_3_away()
{
    static const uint8_t max_value = 63;
    static const uint8_t max_distance_layer_2 = 100;
    static const int r_menjauhi_teman_layer_3 = 60;
    static const uint8_t max_value_layer_3 = 63;
    static const uint8_t max_distance_layer_3 = 30;

    // Ketika saya sebagai attacker, maka teman adalah def_1
    if (robot_saya_role == 1) {
        teman_x = shared_robot_data[bs_data.n_robot_def_1].pose.x;
        teman_y = shared_robot_data[bs_data.n_robot_def_1].pose.y;
    } else {
        teman_x = shared_robot_data[bs_data.n_robot_att].pose.x;
        teman_y = shared_robot_data[bs_data.n_robot_att].pose.y;
    }

    mutex_obs_merged.lock();
    for (int i = 0; i < obs_merged.size(); i++) {
        float obs_x = obs_merged[i].x;
        float obs_y = obs_merged[i].y;

        if (sqrtf((teman_x - obs_x) * (teman_x - obs_x) + (teman_y - obs_y) * (teman_y - obs_y)) < 100)
            continue;

        if (obs_x < FIELD_X_0 + 100 || obs_x > FIELD_X_1 - 100 || obs_y < FIELD_Y_0 + 100 || obs_y > FIELD_Y_1 - 100)
            continue;

        // Dibedakan dua robot agar area hadang lebih luas
        if (robot_saya_role == 1)
            make_tangentCircle_polygon(gawang_saya_kanan_x, gawang_saya_kanan_y, obs_x, obs_y, 150, 1);
        else
            make_tangentCircle_polygon(gawang_saya_kiri_x, gawang_saya_kiri_y, obs_x, obs_y, 200, 1);
        circle(layer_2_raw, Point(obs_x, obs_y), 60, Scalar(255), -1);
    }

    mutex_obs_merged.unlock();

    if (ball.status > 0) {
        if (robot_saya_role == 1)
            make_tangentCircle_polygon(gawang_saya_kanan_x, gawang_saya_kanan_y, ball.x, ball.y, 150, 1);
        else
            make_tangentCircle_polygon(gawang_saya_kiri_x, gawang_saya_kiri_y, ball.x, ball.y, 200, 1);
    }

    Mat buffer = layer_2_raw.clone();
    bitwise_not(buffer, buffer);
    distanceTransform(buffer, buffer, DIST_L2, 3);
    threshold(buffer, buffer, max_distance_layer_2, 255, THRESH_TRUNC);
    normalize(buffer, buffer, 0, 255, NORM_MINMAX, CV_8UC1);
    bitwise_not(buffer, buffer);
    normalize(buffer, layer_2_gradient, 0, max_value, NORM_MINMAX, CV_8UC1);

    // ---------------------------------

    bitwise_not(layer_3_raw, layer_3_raw);

    //     DIST_USER    = -1,  //!< User defined distance
    //     DIST_L1      = 1,   //!< distance = |x1-x2| + |y1-y2|
    //     DIST_L2      = 2,   //!< the simple euclidean distance
    //     DIST_C       = 3,   //!< distance = max(|x1-x2|,|y1-y2|)
    Mat buffer_layer_3 = layer_3_raw.clone();
    bitwise_not(buffer_layer_3, buffer_layer_3);
    distanceTransform(buffer_layer_3, buffer_layer_3, DIST_L2, 3);
    threshold(buffer_layer_3, buffer_layer_3, max_distance_layer_3, 255, THRESH_TRUNC);
    normalize(buffer_layer_3, buffer_layer_3, 0, 255, NORM_MINMAX, CV_8UC1);
    bitwise_not(buffer_layer_3, buffer_layer_3);

    // ---------------------------------

    normalize(buffer_layer_3, layer_3_gradient, 0, max_value_layer_3, NORM_MINMAX, CV_8UC1);
}

void make_layer_4()
{
    static const uint8_t max_value = 63;
    static const uint8_t max_distance_layer_2 = 100;
    static const int r_menjauhi_teman_layer_3 = 60;
    static const uint8_t max_value_layer_4 = 63;
    static const uint8_t max_distance_layer_4 = 30;

    // Ketika saya sebagai attacker, maka teman adalah def_1
    if (robot_saya_role == 1) {
        teman_x = shared_robot_data[bs_data.n_robot_def_1].pose.x;
        teman_y = shared_robot_data[bs_data.n_robot_def_1].pose.y;
    } else {
        teman_x = shared_robot_data[bs_data.n_robot_att].pose.x;
        teman_y = shared_robot_data[bs_data.n_robot_att].pose.y;
    }

    uint8_t obs_ditemukan = 0;
    mutex_obs_merged.lock();
    for (int i = 0; i < obs_merged.size(); i++) {
        float obs_x = obs_merged[i].x;
        float obs_y = obs_merged[i].y;

        if (sqrtf((teman_x - obs_x) * (teman_x - obs_x) + (teman_y - obs_y) * (teman_y - obs_y)) < 100)
            continue;

        // Dibedakan dua robot agar area hadang lebih luas
        make_tangentCircle_polygon(teman_x, teman_y, obs_x, obs_y, 150, 1, 4);

        obs_ditemukan = 1;
    }

    mutex_obs_merged.unlock();

    // ---------------------------------

    if (obs_ditemukan == 1)
        bitwise_not(layer_4_raw, layer_4_raw);

    //     DIST_USER    = -1,  //!< User defined distance
    //     DIST_L1      = 1,   //!< distance = |x1-x2| + |y1-y2|
    //     DIST_L2      = 2,   //!< the simple euclidean distance
    //     DIST_C       = 3,   //!< distance = max(|x1-x2|,|y1-y2|)
    Mat buffer_layer_4 = layer_4_raw.clone();
    bitwise_not(buffer_layer_4, buffer_layer_4);
    distanceTransform(buffer_layer_4, buffer_layer_4, DIST_L2, 3);
    threshold(buffer_layer_4, buffer_layer_4, max_distance_layer_4, 255, THRESH_TRUNC);
    normalize(buffer_layer_4, buffer_layer_4, 0, 255, NORM_MINMAX, CV_8UC1);
    bitwise_not(buffer_layer_4, buffer_layer_4);

    // ---------------------------------

    normalize(buffer_layer_4, layer_4_gradient, 0, max_value_layer_4, NORM_MINMAX, CV_8UC1);
}

void make_tangentCircle_polygon(float x1, float y1, float x2, float y2, float radius, uint8_t full, uint8_t layer_target)
{
    vector<float> tangent_titik_ke_titik;

    tangent_titik_ke_titik = tangentCircle(Point2f(x1, y1), Point2f(x2, y2), radius);

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

        if (m <= 1 && m >= -1) {
            x_tembok_1 = dx > 0 ? 800 : 0;
            y_tembok_1 = m * x_tembok_1 + c;
        } else {
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

        if (m <= 1 && m >= -1) {
            x_tembok_2 = dx > 0 ? 800 : 0;
            y_tembok_2 = m * x_tembok_2 + c;
        } else {
            y_tembok_2 = dy > 0 ? 1200 : 0;
            x_tembok_2 = (y_tembok_2 - c) / m;
        }
    }

    vector<Point> pts;
    pts.push_back(Point(tangent_titik_ke_titik[0], tangent_titik_ke_titik[1]));
    if (full == 1)
        pts.push_back(Point(x1, y1));
    pts.push_back(Point(tangent_titik_ke_titik[2], tangent_titik_ke_titik[3]));
    pts.push_back(Point(x_tembok_2, y_tembok_2));
    pts.push_back(Point(x_tembok_1, y_tembok_1));

    if (layer_target == 3) {
        make_polygon(layer_3_raw, pts, 255);
    } else if (layer_target == 4) {
        make_polygon(layer_4_raw, pts, 255);
    } else {
        make_polygon(layer_3_raw, pts, 255);
    }
}

void make_polygon(Mat& _frame, vector<Point>& _pts, uint8_t val)
{
    vector<vector<Point>> _ptss;
    _ptss.push_back(_pts);
    fillPoly(_frame, _ptss, Scalar(val));
}

vector<float> tangentCircle(const Point2f& mouse, const Point2f& center, double radius)
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

Point get_min_value(Mat& _frame)
{
    Point ret;

    double minVal, maxVal;
    Point minLoc, maxLoc;
    minMaxLoc(layer_total, &minVal, &maxVal, &minLoc, &maxLoc);

    Mat minimum;
    threshold(layer_total, minimum, minVal, 255, THRESH_BINARY_INV);

    // ============================================= MENCARI CENTROID ========
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(minimum, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

    double largest_area = 0;
    int largest_contour_index = 0;

    for (size_t i = 0; i < contours.size(); i++) {
        double area = contourArea(contours[i]);
        if (area > largest_area) {
            largest_area = area;
            largest_contour_index = i;
        }
    }

    vector<Moments> mu(contours.size());
    for (int i = 0; i < contours.size(); i++) {
        mu[i] = moments(contours[i], false);
    }

    vector<Point2f> mc(contours.size());
    for (int i = 0; i < contours.size(); i++) {
        mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
    }

    // MENCARI CENTROID TERDEKAT DENGAN ROBOT
    float min_dist = __FLT_MAX__;
    for (int i = 0; i < mc.size(); i++) {
        float dx = shared_robot_data[robot_saya_num].pose.x - mc[i].x;
        float dy = shared_robot_data[robot_saya_num].pose.y - mc[i].y;

        float curr_dist = sqrtf(dx * dx + dy * dy);

        if (curr_dist < min_dist) {
            ret.x = mc[i].x;
            ret.y = mc[i].y;
            min_dist = curr_dist;
        }
    }

    // Mencari centroid pada contour terbesar kalau preparation
    if (game_status > 0 && game_status <= 127) {
        ret.x = mc[largest_contour_index].x;
        ret.y = mc[largest_contour_index].y;
    }

    return ret;
}

void readU16Bit(uint8_t* dst, const uint16_t* src, uint8_t total_bit, uint8_t offset_bit)
{
    *dst = 0;
    for (uint8_t i = 0; i < total_bit; i++) {
        *dst |= (((*src & (1 << (i + offset_bit))) >> i + offset_bit) << i);
    }
}
