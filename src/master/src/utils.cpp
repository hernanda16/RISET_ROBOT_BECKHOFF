#include "master/master.hpp"

uint64_t getTimeNowUSec()
{
    timeval tim;
    gettimeofday(&tim, NULL);
    return 1.0e3 * tim.tv_sec + tim.tv_usec * 1.0e-3;
}

uint64_t getTimeNowMSec()
{
    timeval tim;
    gettimeofday(&tim, NULL);
    return 1.0e6 * tim.tv_sec + tim.tv_usec;
}

uint64_t getTimeNowSec()
{
    timeval tim;
    gettimeofday(&tim, NULL);
    return tim.tv_sec;
}

float pythagoras(point2d_t point1, point2d_t point2)
{
    return sqrt(pow(point2.x - point1.x, 2) + pow(point2.y - point1.y, 2));
}

float pythagoras(float x0, float y0, float x1, float y1)
{
    return sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2));
}

int16_t kbhit()
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

void readU16Bit(uint8_t *dst, const uint16_t *src, uint8_t total_bit, uint8_t offset_bit)
{
    *dst = 0;
    for (uint8_t i = 0; i < total_bit; i++)
    {
        *dst |= (((*src & (1 << (i + offset_bit))) >> i + offset_bit) << i);
    }
}

float robotAngletoOwnGoal(pose2d_t final_pose)
{
    return angleToTarget(FIELD_X_1_2, FIELD_Y_0, final_pose);
}

float robotAngletoEnemyGoal(pose2d_t final_pose, uint8_t shoot_the_ball)
{
    // if (!shoot_the_ball)
    return angleToTarget(FIELD_X_1_2, FIELD_Y_1, final_pose);

    // float target_goal[2];
    // float dtheta_tolerance;

    // dtheta_tolerance = deltaAngle(min_tolerance_empty_goal_on_field[2], max_tolerance_empty_goal_on_field[2]);
    // if (empty_goal_status == 1) {
    //     target_goal[0] = empty_goal_on_field[0];
    //     target_goal[1] = empty_goal_on_field[1];
    //     // LogWithColor("magenta", "EMPTY GOAL target %d %.2f %.2f || toleransi_min %.2f %.2f || toleransi_max %.2f %.2f delta (%.2f)\n",
    //     empty_goal_status, target_goal[0], target_goal[1], min_tolerance_empty_goal_on_field[0], min_tolerance_empty_goal_on_field[1], max_tolerance_empty_goal_on_field[0], max_tolerance_empty_goal_on_field[1], deltaAngle(min_tolerance_empty_goal_on_field[2], max_tolerance_empty_goal_on_field[2]);
    // } else {
    //     if (final_pose.x < FIELD_X_1_2) {
    //         target_goal[0] = 460;
    //         target_goal[1] = 1200;
    //         // LogWithColor("green", "EMPTY GOAL %d %.2f %.2f\n", empty_goal_status, target_goal[0], target_goal[1]);
    //     } else {
    //         target_goal[0] = 340;
    //         target_goal[1] = 1200;
    //         // LogWithColor("green", "EMPTY GOAL %d %.2f %.2f\n", empty_goal_status, target_goal[0], target_goal[1]);
    //     }
    // }

    // return angleToTarget(target_goal[0], target_goal[1], final_pose);
}

float angleToTarget(float target_x, float target_y, pose2d_t final_pose)
{
    float dx = target_x - final_pose.x;
    float dy = target_y - final_pose.y;

    return atan2(dy, dx) RAD2DEG;
}

void kickerControl(pc2stm_t *stm_data, uint8_t mode, float power, uint16_t position)
{
    stm_data->kicker_mode = mode;
    stm_data->kicker_power = power;
    stm_data->kicker_position = position;
}

void buzzerControl(pc2stm_t *stm_data, uint8_t cnt, uint8_t time)
{
    stm_data->buzzer_cnt = cnt;
    stm_data->buzzer_time = time;
}

float deltaAngle(float angle1, float angle2, bool absolute)
{
    float ret = angle1 - angle2;
    (ret > 180) ? ret -= 360 : (ret < -180) ? ret += 360
                                            : ret;
    if (absolute)
        return fabs(ret);
    else
        return ret;
}

void mirrorPosX(float *pos_x)
{
    *pos_x = 800 - *pos_x;
}

void LogWithColor(std::string color, const char *text, ...)
{
    if (color == "red")
        printf("\033[1;31m");
    else if (color == "green")
        printf("\033[1;32m");
    else if (color == "yellow")
        printf("\033[1;33m");
    else if (color == "blue")
        printf("\033[1;34m");
    else if (color == "magenta")
        printf("\033[1;35m");
    else if (color == "cyan")
        printf("\033[1;36m");
    else if (color == "white")
        printf("\033[1;37m");
    else if (color == "reset")
        printf("\033[0m");
    else
        printf("\033[0m");

    va_list args;
    va_start(args, text);
    vprintf(text, args);
    va_end(args);
    printf("\033[0m");
}