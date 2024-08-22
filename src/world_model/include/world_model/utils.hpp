#ifndef UTILS_HPP
#define UTILS_HPP

#define DEG2RAD *0.017453292519943295
#define RAD2DEG *57.29577951308232

#define FIELD_MAX_X 950
#define FIELD_MIN_X -150
#define FIELD_MAX_Y 1350
#define FIELD_MIN_Y -150

#include "sys/ioctl.h"
#include "termios.h"
#include <cstring>
#include <fstream>
#include <math.h>
#include <ros/package.h>
#include <stdint.h>

typedef struct
{
    float x;
    float y;
    float theta;
} pose2d_t;

typedef struct
{
    uint16_t curr_px;
    uint16_t prev_px;
    int16_t speed;
} enc_t;

typedef struct
{
    float curr_px;
    float prev_px;
    float speed;
} gyro_t;

typedef struct
{
    float x;
    float y;
} point2d_t;

typedef struct
{
    float r;
    float theta;
} polar2d_t;

typedef struct
{
    point2d_t position;
    uint16_t vote_count;
} balls_vote_t;

typedef struct
{
    pose2d_t pose;
    pose2d_t vel;
} element_t; // Velocity constant element

typedef struct
{
    uint8_t role;
    pose2d_t pose;
    uint16_t robot_condition;
    point2d_t ball;
    uint8_t ball_status;
} shrobot_data_t;

int16_t center_cam_x;
int16_t center_cam_y;

float pythagoras(point2d_t point1, point2d_t point2)
{
    return sqrt(pow(point2.x - point1.x, 2) + pow(point2.y - point1.y, 2));
}

float pythagoras(float x0, float y0, float x1, float y1)
{
    return sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2));
}

bool isInsideField(point2d_t point, int field_max_x = FIELD_MAX_X, int field_min_x = FIELD_MIN_X, int field_max_y = FIELD_MAX_Y, int field_min_y = FIELD_MIN_Y)
{
    return (point.x > field_min_x && point.x < field_max_x && point.y > field_min_y && point.y < field_max_y);
}

bool isInsideField(float x, float y, int field_max_x = FIELD_MAX_X, int field_min_x = FIELD_MIN_X, int field_max_y = FIELD_MAX_Y, int field_min_y = FIELD_MIN_Y)
{
    return (x > field_min_x && x < field_max_x && y > field_min_y && y < field_max_y);
}

void frameToWorld(point2d_t frame_pixel, point2d_t* world_point, polar2d_t* world_polar, pose2d_t* robot_pose, int16_t* LUTfr2lap)
{
    float dist_px = pythagoras(frame_pixel.x, frame_pixel.y, 0, 0);
    float angle_px = atan2(frame_pixel.y, frame_pixel.x) * 180 / M_PI;

    // if (dist_px < 75)
    //     dist_px = 75;
    // else if (dist_px > 319)
    //     dist_px = 319;

    while (angle_px >= 360)
        angle_px -= 360;
    while (angle_px < 0)
        angle_px += 360;

    /* This is coordinate that just based on robot */
    world_polar->r = LUTfr2lap[(int)(angle_px) * 3200 + (int)(dist_px * 10)];

    // TITIP
    // world_polar->r = LUTfr2lap[(int)(90) * 3200 + (int)(dist_px * 10)];

    world_polar->theta = angle_px + robot_pose->theta - 90;

    /* Turning ball coordinate based on robot into based on field */
    world_point->x = robot_pose->x + world_polar->r * cosf(world_polar->theta DEG2RAD);
    world_point->y = robot_pose->y + world_polar->r * sinf(world_polar->theta DEG2RAD);
}

void worldToFrame(point2d_t world_point, pose2d_t robot_pose, int16_t* LUTlap2fr, point2d_t* frame_pixel)
{
    polar2d_t cam_polar;
    cam_polar.r = sqrtf((world_point.x - robot_pose.x) * (world_point.x - robot_pose.x) + (world_point.y - robot_pose.y) * (world_point.y - robot_pose.y));
    cam_polar.theta = atan2(world_point.y - robot_pose.y, world_point.x - robot_pose.x) RAD2DEG;

    if (cam_polar.theta < 0)
        cam_polar.theta += 360;
    else if (cam_polar.theta >= 360)
        cam_polar.theta -= 360;

    float dist_px = LUTlap2fr[(int)(cam_polar.theta) * 1200 + (int)(cam_polar.r)] * 0.1;

    frame_pixel->x = dist_px * cosf((cam_polar.theta - robot_pose.theta + 90) DEG2RAD) + center_cam_x;
    frame_pixel->y = center_cam_y - dist_px * sinf((cam_polar.theta - robot_pose.theta + 90) DEG2RAD);
}

void loadLUT(int16_t* lut_src2dst, bool is_from_frame = true)
{
    char LUT_file[100];
    char* robot_num;
    robot_num = getenv("ROBOT");
    std::string current_dir = ros::package::getPath("world_model");

    if (is_from_frame)
        sprintf(LUT_file, "%s/../../config/IRIS%s/lut_px2cm_msi_kri2.bin", current_dir.c_str(), robot_num);
    // sprintf(LUT_file, "%s/../../config/IRIS%s/kri/azzam_lut1000.bin", current_dir.c_str(), robot_num);
    // sprintf(LUT_file, "%s/../../config/IRIS%s/kri/kri_msi_lut100.bin", current_dir.c_str(), robot_num);

    // sprintf(LUT_file, "%s/../../config/IRIS%s/data_nuc_05_lut.bin", current_dir.c_str(), robot_num);

    else
        // sprintf(LUT_file, "%s/../../config/IRIS%s/lut_cm2px.bin", current_dir.c_str(), robot_num);
        // sprintf(LUT_file, "%s/../../config/IRIS%s/data_nuc_cm2px.bin", current_dir.c_str(), robot_num);
        sprintf(LUT_file, "%s/../../config/IRIS%s/lut_cm2px_msi_kri2.bin", current_dir.c_str(), robot_num);

    std::ifstream lut_src2dst_fs(LUT_file, std::ios::binary | std::ios::in);

    char px2cm_buffer[360 * 3200 * 2];
    char cm2px_buffer[360 * 1200 * 2];

    if (is_from_frame) {
        lut_src2dst_fs.read((char*)px2cm_buffer, 360 * 3200 * 2);
        lut_src2dst_fs.close();
        memcpy(lut_src2dst, px2cm_buffer, sizeof(px2cm_buffer));
    } else {
        lut_src2dst_fs.read((char*)cm2px_buffer, 360 * 1200 * 2);
        lut_src2dst_fs.close();
        memcpy(lut_src2dst, cm2px_buffer, sizeof(cm2px_buffer));
    }
}

int16_t kbhit()
{
    static const int STDIN = 0;
    static bool initialized = false;

    if (!initialized) {
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

#endif