#include "master/motion.hpp"

using namespace ruckig;

void Motion::init(pose2d_t* final_pose, vel2d_t* act_vel, vel2d_t* world_vel, ball_t* ball, pc2stm_t* stm_data, std::vector<point2d_t>* waypoints_ball, std::vector<point2d_t>* obstacle, std::vector<point2d_t>* obstacle_pcl)
{
    this->final_pose = final_pose;
    this->act_vel = act_vel;
    this->world_vel = world_vel;
    this->ball = ball;
    this->stm_data = stm_data;
    this->waypoints_ball = waypoints_ball;
    this->obstacle = obstacle;
    this->obstacle_pcl = obstacle_pcl;

    translation_profile_.max_velocity = 180; // 300 || 100
    translation_profile_.max_acceleration = 200; // 250 || 100
    translation_profile_.max_jerk = 500;
    // translation_profile_.max_velocity = 100;
    // translation_profile_.max_acceleration = 100;
    // translation_profile_.max_jerk = 500;x

    translation_profile_with_ball.max_velocity = 180; // 180 || 100
    translation_profile_with_ball.max_acceleration = 180; // 210 || 100
    translation_profile_with_ball.max_jerk = 500;

    angular_profile_.max_velocity = 250;
    angular_profile_.max_acceleration = 180;
    angular_profile_.max_jerk = 500;

    angular_profile_with_ball.max_velocity = 250;
    angular_profile_with_ball.max_acceleration = 140;
    angular_profile_with_ball.max_jerk = 500;

    return;
}

void Motion::loadConfig()
{
    char* robot_num;
    robot_num = getenv("ROBOT");
    char config_file[100];
    std::string current_dir = ros::package::getPath("vision");
    sprintf(config_file, "%s/../../config/IRIS%s/dynamic_conf.yaml", current_dir.c_str(), robot_num);

    YAML::Node config = YAML::LoadFile(config_file);
    center_cam[0] = config["Camera"]["cam_offset_x"].as<int>();
    center_cam[1] = config["Camera"]["cam_offset_y"].as<int>();

    center_cam[0] += (int)(g_res_x * 0.5);
    center_cam[1] += (int)(g_res_y * 0.5);
}

void Motion::manualMotion(float vel_x, float vel_y, float vel_theta, bool is_ruckig)
{
    float acc_max = 5.1123; // 3.5123
    static vel2d_t buffer_vel = { 0, 0, 0 };

    // if (vel_x == 0 && vel_y == 0 && vel_theta == 0) {
    //     acc_max = 4.5123;
    // }

    polar2d_t polar_acc;
    float dx = vel_x - buffer_vel.x;
    float dy = vel_y - buffer_vel.y;
    polar_acc.r = sqrt(pow(dx, 2) + pow(dy, 2));
    polar_acc.theta = atan2(dy, dx);

    if (ball->is_caught > 0) {
        if (polar_acc.r > acc_max * 0.67) {
            polar_acc.r = acc_max * 0.67;
        }
    } else {
        if (polar_acc.r > acc_max * 1) {
            polar_acc.r = acc_max * 1;
        }
    }

    static float prev_vtheta = 0;

    float d_theta = vel_theta - buffer_vel.theta;

    // printf("%.2f %.2f %.2f\n", buffer_vel.theta, vel_theta, d_theta);

    if (d_theta > 5.3 && ball->is_caught > 0) {
        d_theta = 5.3;
    } else if (d_theta < -5.3 && ball->is_caught > 0) {
        d_theta = -5.3;
    }

    prev_vtheta = vel_theta;

    buffer_vel.x += polar_acc.r * cos(polar_acc.theta);
    buffer_vel.y += polar_acc.r * sin(polar_acc.theta);
    buffer_vel.theta += d_theta;

    // printf("buffer_vel.x : %.1f buffer_vel.y : %.1f buffer_vel.theta : %.1f\n", buffer_vel.x, buffer_vel.y, buffer_vel.theta);
    if (use_acceleration_constant == 1) {
        act_vel->x = vel_x;
        act_vel->y = vel_y;
        act_vel->theta = vel_theta;
    } else {
        act_vel->x = buffer_vel.x;
        act_vel->y = buffer_vel.y;
        act_vel->theta = buffer_vel.theta;
    }

    if (is_ruckig) {
        act_vel->x = vel_x;
        act_vel->y = vel_y;
    }

    return;
}

int8_t Motion::motionToPoint(float target_x, float target_y, float target_theta, uint16_t minmax_vel_linear, uint16_t minmax_vel_angular, uint8_t flag)
{
    // TODO : Make Config
    static PID linear_pid(1.8, 0, 0); // 1.8, 0, 0
    static PID angular_pid(3, 0, 0); // 1.8, 0, 0.5

    static pose2d_t last_target;
    memset(&last_target, 0, sizeof(pose2d_t));

    if (last_target.x != target_x || last_target.y != target_y || last_target.theta != target_theta) {
        linear_pid.reset();
        angular_pid.reset();

        last_target.x = target_x;
        last_target.y = target_y;
        last_target.theta = target_theta;
    }

    float error_x = 0;
    float error_y = 0;
    float error_linear = 0;
    float error_angular = 0;

    error_x = target_x - final_pose->x;
    error_y = target_y - final_pose->y;

    error_linear = sqrtf(error_x * error_x + error_y * error_y);
    error_angular = target_theta - final_pose->theta;

    while (error_angular > 180)
        error_angular -= 360;
    while (error_angular < -180)
        error_angular += 360;

    vel2d_t output_vel;
    float buffer_linear_vel;
    buffer_linear_vel = linear_pid.calculate(error_linear, minmax_vel_linear);
    output_vel.theta = angular_pid.calculate(error_angular, minmax_vel_angular);

    output_vel.x = buffer_linear_vel * cosf(atan2(error_y, error_x));
    output_vel.y = buffer_linear_vel * sinf(atan2(error_y, error_x));

    float output_x = output_vel.x;
    float output_y = output_vel.y;
    // float output_theta = output_vel.theta;

    if ((flag & NORMAL_OBSTACLE) == NORMAL_OBSTACLE && (flag & NORMAL_OBSTACLE_BALL) == NORMAL_OBSTACLE_BALL) {
        output_x = ObstacleAvoidance(output_vel.x, output_vel.y, 90, 200, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | (flag & PREDICT)).vx;
        output_y = ObstacleAvoidance(output_vel.x, output_vel.y, 90, 200, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL | (flag & PREDICT)).vy;
    } else if ((flag & NORMAL_OBSTACLE) == NORMAL_OBSTACLE) {
        // ...
        if (((flag & INVERT_OBS) == INVERT_OBS)) {
            avoid_obs_t obs_partial = ObstacleAvoidance(output_vel.x, output_vel.y, 50, 200, NORMAL_OBSTACLE | (flag & PREDICT));
            avoid_obs_t obs_full = ObstacleAvoidance(output_vel.x, output_vel.y, 180, 200, NORMAL_OBSTACLE | (flag & PREDICT));

            if (obs_partial.status || obs_full.status) {
                float error_posisi_obstacle = obs_partial.distance - 50;
                float error_sudut_obstacle = obs_full.angle - final_pose->theta - 180;
                while (error_sudut_obstacle < -180)
                    error_sudut_obstacle += 360;
                while (error_sudut_obstacle > 180)
                    error_sudut_obstacle -= 360;

                float output_posisi_obstacle = linear_pid.calculate(error_posisi_obstacle, minmax_vel_linear);
                float output_sudut_obstacle = angular_pid.calculate(error_sudut_obstacle, minmax_vel_angular);

                if (obs_partial.status) {
                    output_x = output_posisi_obstacle * cosf(obs_partial.angle DEG2RAD) + obs_partial.vx;
                    output_y = output_posisi_obstacle * sinf(obs_partial.angle DEG2RAD) + obs_partial.vy;
                }

                output_vel.theta = output_sudut_obstacle;
            }
        }
        // BIASA
        else {
            output_x = ObstacleAvoidance(output_vel.x, output_vel.y, 90, 100, NORMAL_OBSTACLE | (flag & PREDICT)).vx;
            output_y = ObstacleAvoidance(output_vel.x, output_vel.y, 90, 100, NORMAL_OBSTACLE | (flag & PREDICT)).vy;
        }
    } else if ((flag & NORMAL_OBSTACLE_BALL) == NORMAL_OBSTACLE_BALL) {
        output_x = ObstacleAvoidance(output_vel.x, output_vel.y, 90, 170, NORMAL_OBSTACLE_BALL | (flag & PREDICT)).vx;
        output_y = ObstacleAvoidance(output_vel.x, output_vel.y, 90, 170, NORMAL_OBSTACLE_BALL | (flag & PREDICT)).vy;
    }

    if ((flag & NORMAL_OBSTACLE_FRIEND) == NORMAL_OBSTACLE_FRIEND) {
        output_x = ObstacleAvoidance(output_vel.x, output_vel.y, 90, 170, flag).vx;
        output_y = ObstacleAvoidance(output_vel.x, output_vel.y, 90, 170, flag).vy;
    }

    worldToLocalMotion(output_x, output_y, output_vel.theta);

    // printf("Error Linear : %f Error Angular : %.1f Output vel x y : %.1f %.1f || %.2f %.2f \n", error_linear, error_angular, output_vel.x, output_vel.y, final_pose->x, final_pose->y);

    if (fabs(error_linear) < 10 && fabs(error_angular) < 1) {
        return 1;
    } else {
        return 0;
    }
}

int8_t Motion::motionAroundBall(float target_arc_theta, float robot_target_th, uint16_t radius_point, uint16_t minmax_vel_linear, uint16_t minmax_vel_angular, uint8_t flag)
{
    if (ball->status > 0) {
        return motionAroundPointAngle(ball->pos.x, ball->pos.y, target_arc_theta, robot_target_th, radius_point, minmax_vel_linear, minmax_vel_angular, flag);
    }

    manualMotion(0, 0, 0);
    return false;
}

avoid_obs_t Motion::ObstacleAvoidance(float vx_input, float vy_input, float angle_tolerance, float dist, uint8_t flags)
{
    obstacle_checking_t obs;
    avoid_obs_t ret;

    float angle_target = atan2(vy_input, vx_input) RAD2DEG;
    float translation_vel = sqrt(vx_input * vx_input + vy_input * vy_input);

    obs = obstacleChecking(angle_target, angle_tolerance, dist, flags); // get obs data

    if (obs.status) {
        float delta_angle = angle_target - obs.angle; // to determine closest way to go to target

        // wrap into -180 to 180
        while (delta_angle < -180)
            delta_angle += 360;
        while (delta_angle > 180)
            delta_angle -= 360;

        // if obstacle is in left from robot
        if (delta_angle < 0) {
            // Turn robot to right, (90 degrees from obs)
            ret.vx = translation_vel * cos((obs.angle - 90) DEG2RAD);
            ret.vy = translation_vel * sin((obs.angle - 90) DEG2RAD);
        }
        // if obstacle is in right from robot
        else {
            // Turn robot to left, (90 degrees from obs)
            ret.vx = translation_vel * cos((obs.angle + 90) DEG2RAD);
            ret.vy = translation_vel * sin((obs.angle + 90) DEG2RAD);
        }
    } else // if no obs, then just use current vel
    {
        ret.vx = vx_input;
        ret.vy = vy_input;
    }

    ret.angle = obs.angle;
    ret.distance = obs.distance;
    ret.status = obs.status;

    return ret;
}

int8_t Motion::ruckigToPoint3(float target_x, float target_y, float target_theta, uint16_t minmax_vel_linear, uint16_t minmax_vel_angular, uint8_t flags)
{
    static MachineState state_control;
    state_control.reentry(0, 0.05);

    float x = final_pose->x;
    float y = final_pose->y;
    float theta = final_pose->theta;
    float dx = feedback_vel->x;
    float dy = feedback_vel->y;
    float dtheta = feedback_vel->theta;

    std::vector<float> out_vel(3);

    static Ruckig<3> otg(0.02);
    static InputParameter<3> input;
    static OutputParameter<3> output;

    float dtheta_target = theta - target_theta;
    while (dtheta_target > 180)
        dtheta_target -= 360;
    while (dtheta_target < -180)
        dtheta_target += 360;

    float target_x_baru = target_x;
    float target_y_baru = target_y;
    float target_theta_baru = theta - dtheta_target;

    if ((flags & NORMAL_OBSTACLE) == NORMAL_OBSTACLE || (flags & NORMAL_OBSTACLE_BALL) == NORMAL_OBSTACLE_BALL || (flags & NORMAL_OBSTACLE_FRIEND) == NORMAL_OBSTACLE_FRIEND) {
        float dtarget_x = target_x - x;
        float dtarget_y = target_y - y;
        float arah_jalan = atan2f(dtarget_y, dtarget_x) RAD2DEG;

        obstacle_checking_t obs;

        if ((flags & NORMAL_OBSTACLE) == NORMAL_OBSTACLE)
            obs = obstacleChecking(arah_jalan, 60, 150, flags);
        else if ((flags & NORMAL_OBSTACLE_BALL) == NORMAL_OBSTACLE_BALL)
            obs = obstacleChecking(arah_jalan, 60, 150, flags);
        else
            obs = obstacleChecking(arah_jalan, 60, 150, flags);

        // printf("HELLO %d\n", obs.status);

        if (obs.status) {
            float sudut_obs_ke_robot = angleToTarget(final_pose->x, final_pose->y, obs.pos_x, obs.pos_y);
            float sudut_obs_ke_tujuan = angleToTarget(target_x, target_y, obs.pos_x, obs.pos_y);

            float sudut_selisih = sudut_obs_ke_tujuan - sudut_obs_ke_robot;

            while (sudut_selisih > 180)
                sudut_selisih -= 360;
            while (sudut_selisih < -180)
                sudut_selisih += 360;

            float sudut_menghindar = sudut_obs_ke_robot + sudut_selisih * 0.5;
            float jarak_menghindar = 120;

            target_x_baru = obs.pos_x + jarak_menghindar * cos(sudut_menghindar DEG2RAD);
            target_y_baru = obs.pos_y + jarak_menghindar * sin(sudut_menghindar DEG2RAD);
        }
    }

    input.target_position = { target_x_baru, target_y_baru, target_theta_baru };

    if (ball->is_caught > 0) {
        input.max_velocity = { translation_profile_with_ball.max_velocity, translation_profile_with_ball.max_velocity, angular_profile_with_ball.max_velocity };
        input.max_acceleration = { translation_profile_with_ball.max_acceleration, translation_profile_with_ball.max_acceleration, angular_profile_with_ball.max_acceleration };
    } else {
        input.max_velocity = { translation_profile_.max_velocity, translation_profile_.max_velocity, angular_profile_.max_velocity };
        input.max_acceleration = { translation_profile_.max_acceleration, translation_profile_.max_acceleration, angular_profile_.max_acceleration };
    }

    if (state_control.value == 0) {
        state_control.value = 1;

        input.current_position = { x, y, theta };
        input.current_velocity = { dx, dy, dtheta };
        input.current_acceleration = { 0, 0, 0 };
        input.target_velocity = { 0, 0, 0 };
        input.target_acceleration = { 0, 0, 0 };
    } else {
        output.pass_to_input(input);
        input.current_position = { x, y, theta };
        // input.current_velocity = { dx, dy };
    }

    otg.update(input, output);

    for (int i = 0; i < 3; i++) {
        out_vel[i] = output.new_velocity[i];
    }

    // printf("inp: %.2f %.2f %.2f || trgt: %.2f %.2f %.2f || out: %.2f %.2f %.2f\n",
    //     input.current_position[0], input.current_position[1], input.current_position[2],
    //     input.target_position[0], input.target_position[1], input.target_position[2],
    //     out_vel[0], out_vel[1], out_vel[2]);

    // =================================================================================================================

    worldToLocalMotion(out_vel[0], out_vel[1], out_vel[2], true);

    float error_angular = 0;

    error_angular = target_theta - final_pose->theta;

    while (error_angular > 180)
        error_angular -= 360;
    while (error_angular < -180)
        error_angular += 360;

    float ex = target_x - x;
    float ey = target_y - y;
    float error_xy = sqrtf(ex * ex + ey * ey);

    if (error_xy < 40 && fabs(error_angular) < 1)
        return 1;

    return 0;
}

int8_t Motion::ruckigToPoint(float target_x, float target_y, float target_theta, uint16_t minmax_vel_linear, uint16_t minmax_vel_angular, uint8_t flags)
{
    static MachineState state_control;
    state_control.reentry(0, 0.05);

    float x = final_pose->x;
    float y = final_pose->y;
    float dx = feedback_vel->x;
    float dy = feedback_vel->y;

    std::vector<float> out_vel(3);

    static Ruckig<2> otg(0.02);
    static InputParameter<2> input;
    static OutputParameter<2> output;

    float target_x_baru = target_x;
    float target_y_baru = target_y;
    float target_th_baru = target_theta;

    if ((flags & NORMAL_OBSTACLE) == NORMAL_OBSTACLE || (flags & NORMAL_OBSTACLE_BALL) == NORMAL_OBSTACLE_BALL || (flags & NORMAL_OBSTACLE_FRIEND) == NORMAL_OBSTACLE_FRIEND) {
        float dtarget_x = target_x - x;
        float dtarget_y = target_y - y;
        float arah_jalan = atan2f(dtarget_y, dtarget_x) RAD2DEG;

        obstacle_checking_t obs;

        if ((flags & NORMAL_OBSTACLE) == NORMAL_OBSTACLE)
            obs = obstacleChecking(arah_jalan, 60, 150, flags);
        else if ((flags & NORMAL_OBSTACLE_BALL) == NORMAL_OBSTACLE_BALL)
            obs = obstacleChecking(arah_jalan, 60, 150, flags);
        else
            obs = obstacleChecking(arah_jalan, 60, 150, flags);

        if ((flags & INVERT_OBS) == INVERT_OBS) {
            obstacle_checking_t obs_full = obstacleChecking(arah_jalan, 180, 150, NORMAL_OBSTACLE | (flags & PREDICT));
            if (obs_full.status) {
                target_th_baru = obs_full.angle - 180;
            } else if (obs.status) {
                target_th_baru = obs.angle - 180;
            }
        }

        if (obs.status) {
            float sudut_obs_ke_robot = angleToTarget(final_pose->x, final_pose->y, obs.pos_x, obs.pos_y);
            float sudut_obs_ke_tujuan = angleToTarget(target_x, target_y, obs.pos_x, obs.pos_y);

            float sudut_selisih = sudut_obs_ke_tujuan - sudut_obs_ke_robot;

            while (sudut_selisih > 180)
                sudut_selisih -= 360;
            while (sudut_selisih < -180)
                sudut_selisih += 360;

            float sudut_menghindar = sudut_obs_ke_robot + sudut_selisih * 0.5;
            float jarak_menghindar = 140;

            target_x_baru = obs.pos_x + jarak_menghindar * cos(sudut_menghindar DEG2RAD);
            target_y_baru = obs.pos_y + jarak_menghindar * sin(sudut_menghindar DEG2RAD);
        }
    }

    input.target_position = { target_x_baru, target_y_baru };

    if (ball->is_caught > 0) {
        input.max_velocity = { translation_profile_with_ball.max_velocity, translation_profile_with_ball.max_velocity };
        input.max_acceleration = { translation_profile_with_ball.max_acceleration, translation_profile_with_ball.max_acceleration };
    } else {
        input.max_velocity = { translation_profile_.max_velocity, translation_profile_.max_velocity };
        input.max_acceleration = { translation_profile_.max_acceleration, translation_profile_.max_acceleration };
        input.max_jerk = { translation_profile_.max_jerk, translation_profile_.max_jerk };
    }

    if (state_control.value == 0) {
        state_control.value = 1;

        input.current_position = { x, y };
        input.current_velocity = { dx, dy };
        input.current_acceleration = { 0, 0 };
        input.target_velocity = { 0, 0 };
        input.target_acceleration = { 0, 0 };
    } else {
        output.pass_to_input(input);
        input.current_position = { x, y };
        // input.current_velocity = { dx, dy };
    }

    otg.update(input, output);

    for (int i = 0; i < 2; i++) {
        out_vel[i] = output.new_velocity[i];
    }

    // =================================================================================================================

    static PID angular_pid(3, 0, 0); // 2.5, 0, 0.5
    static float last_target_theta;
    float error_angular = 0;

    // if (last_target_theta != target_th_baru)
    // {
    //     angular_pid.reset();
    //     last_target_theta = target_th_baru;
    // }

    error_angular = target_th_baru - final_pose->theta;

    while (error_angular > 180)
        error_angular -= 360;
    while (error_angular < -180)
        error_angular += 360;

    out_vel[2] = angular_pid.calculate(error_angular, minmax_vel_angular);

    worldToLocalMotion(out_vel[0], out_vel[1], out_vel[2], true);

    float ex = target_x - x;
    float ey = target_y - y;
    float error_xy = sqrtf(ex * ex + ey * ey);

    if (error_xy < 40 && fabs(error_angular) < 1)
        return 1;

    return 0;
}

int8_t Motion::motionToPointRecvBall(float target_x, float target_y, float target_theta, uint16_t minmax_vel_linear, uint16_t minmax_vel_angular, uint8_t flag, float ball_vx, float ball_vy)
{
    // TODO : Make Config
    // static PID linear_pid(1.8, 0, 0);    // 1.8, 0, 0
    // static PID angular_pid(1.8, 0, 0.5); // 1.8, 0, 0.5

    // asd
    static PID linear_pid(1.9, 0, 0); // 1.8, 0, 0
    static PID angular_pid(1.8, 0, 0); // 1.8, 0, 0.5

    static pose2d_t last_target;
    memset(&last_target, 0, sizeof(pose2d_t));

    // if (last_target.x != target_x || last_target.y != target_y || last_target.theta != target_theta) {
    //     linear_pid.reset();
    //     angular_pid.reset();

    //     last_target.x = target_x;
    //     last_target.y = target_y;
    //     last_target.theta = target_theta;
    // }

    float error_x = 0;
    float error_y = 0;
    float error_linear = 0;
    float error_angular = 0;

    error_x = target_x - final_pose->x;
    error_y = target_y - final_pose->y;

    error_linear = sqrtf(error_x * error_x + error_y * error_y);
    error_angular = target_theta - final_pose->theta;

    (error_angular > 180) ? error_angular -= 360 : (error_angular < -180) ? error_angular += 360
                                                                          : error_angular;

    vel2d_t output_vel;
    float buffer_linear_vel;
    buffer_linear_vel = linear_pid.calculate(error_linear, minmax_vel_linear);
    output_vel.theta = angular_pid.calculate(error_angular, minmax_vel_angular);

    output_vel.x = buffer_linear_vel * cosf(atan2(error_y, error_x));
    output_vel.y = buffer_linear_vel * sinf(atan2(error_y, error_x));

    // worldToLocalMotion(output_vel.x, output_vel.y, output_vel.theta);

    float vx_local = output_vel.x * sinf(final_pose->theta DEG2RAD) - output_vel.y * cosf(final_pose->theta DEG2RAD);
    float vy_local = output_vel.x * cosf(final_pose->theta DEG2RAD) + output_vel.y * sinf(final_pose->theta DEG2RAD);

    float vx_bola_local = ball_vx * sinf(final_pose->theta DEG2RAD) - ball_vy * cosf(final_pose->theta DEG2RAD);
    float vy_bola_local = ball_vx * cosf(final_pose->theta DEG2RAD) + ball_vy * sinf(final_pose->theta DEG2RAD);

    printf("VV: %.2f %.2f || %.2f %.2f || %.2f %.2f %.2f %.2f\n",
        output_vel.x, output_vel.y, ball_vx, ball_vy,
        vx_local, vy_local, vx_bola_local, vy_bola_local);

    if (flag == 1) {
        // vy_local *= 0.1;
        manualMotion(vx_local, /*vy_bola_local - */ vy_local * 0.1, output_vel.theta);
    } else {
        manualMotion(vx_local, vy_local, output_vel.theta);
    }

    if (fabs(error_linear) < 20 && fabs(error_angular) < 5) {
        return 1;
    } else {
        return 0;
    }
}

uint8_t Motion::motionAngleControl(float vx_local, float vy_local, float target_theta, float vtheta)
{
    static PID angular_pid(1.8, 0, 0.5);

    float error_angular = target_theta - final_pose->theta;
    if (error_angular > 180)
        error_angular -= 360;
    else if (error_angular < -180)
        error_angular += 360;

    float output_angular = angular_pid.calculate(error_angular, vtheta);

    manualMotion(vx_local, vy_local, output_angular);

    if (fabs(error_angular) < 5) {
        return 1;
    } else {
        return 0;
    }
}
int8_t Motion::pivotMotion(float target_th, float vth, float kp_angular)
{
    static PID angular_pid(kp_angular, 0, 0); // 3

    float error_th = target_th - final_pose->theta;

    while (error_th > 180)
        error_th -= 360;
    while (error_th < -180)
        error_th += 360;

    float output_theta = angular_pid.calculate(error_th, vth);

    float output_x = output_theta * M_PI / 180 * 30;
    float output_y = fabs(output_x * 1);

    if (output_y > 100)
        output_y = 100;
    else if (output_y < -100)
        output_y = -100;

    manualMotion(output_x, output_y, output_theta);

    printf("error: %.2f %.2f %.2f || %.2f %.2f\n", target_th, final_pose->theta, error_th, output_x, output_y);

    if (fabs(error_th) < 2)
        return 1;
    else
        return 0;
}

int8_t Motion::newMotionToPoint(float target_x, float target_y, float target_theta, uint16_t minmax_vel_linear, uint16_t minmax_vel_angular)
{
    // std::priority_queue<std::pair<float, std::pair<float, float>>, std::vector<std::pair<float, std::pair<float, float>>>, std::greater<std::pair<float, std::pair<float, float>>>> obs_pq;
    // float new_target_x = target_x;
    // float new_target_y = target_y;

    // float angle_robot_to_target = angleToTarget(target_x, target_y);

    // // TODO: make config
    // float threshold_angle = 45;

    // for (size_t obs_it = 0; obs_it < obstacle->size(); obs_it++)
    // {
    //     float obs_distance = pythagoras(final_pose->x, final_pose->y, obstacle->at(obs_it).x, obstacle->at(obs_it).y);
    //     obs_pq.push({obs_distance, {obstacle->at(obs_it).x, obstacle->at(obs_it).y}});
    // }

    // printf("Obstacle Size : %d\n", obs_pq.size());

    // while (!obs_pq.empty())
    // {
    //     std::pair<float, std::pair<float, float>> nearest_obs = obs_pq.top();
    //     printf("Distance : %.1f X : %.1f Y : %.1f\n", nearest_obs.first, nearest_obs.second.first, nearest_obs.second.second);

    //     float angle_robot_to_obs = angleToTarget(nearest_obs.second.first, nearest_obs.second.second);
    //     float angle_diff = deltaAngle(angle_robot_to_obs, final_pose->theta);
    //     float orthogonal_projection = nearest_obs.first * cos(angle_diff DEG2RAD);

    //     // TODO: make config
    //     float threshold_distance = 150;

    //     if (orthogonal_projection > threshold_distance)
    //     {
    //         obs_pq.pop();
    //         continue;
    //     }

    //     if (fabs(angle_robot_to_target - angle_robot_to_obs) > threshold_angle)
    //     {
    //         obs_pq.pop();
    //         continue;
    //     }

    //     float angle_obs_to_robot = angleToTarget(final_pose->x, final_pose->y, nearest_obs.second.first, nearest_obs.second.second);
    //     float angle_obs_to_target = angleToTarget(target_x, target_y, nearest_obs.second.first, nearest_obs.second.second);
    //     float bisection_angle = (angle_obs_to_robot + angle_obs_to_target) / 2;

    //     new_target_x = nearest_obs.second.first + threshold_distance * cos(bisection_angle DEG2RAD);
    //     new_target_y = nearest_obs.second.second + threshold_distance * sin(bisection_angle DEG2RAD);
    //     break;
    // }

    // return motionToPoint(new_target_x, new_target_y, target_theta, minmax_vel_linear, minmax_vel_angular);

    //=================================================
    float min_dist = FLT_MAX;
    float x_nearest_obs = 0;
    float y_nearest_obs = 0;
    uint8_t obs_ditemukan = 0;

    float sudut_robot_ke_tujuan = angleToTarget(target_x, target_y, final_pose->x, final_pose->y);

    /*Hill Obstacle*/
    for (size_t obs_it = 0; obs_it < obstacle_pcl->size(); obs_it++) {
        float jarak_robot_ke_obs = pythagoras(final_pose->x, final_pose->y, obstacle_pcl->at(obs_it).x, obstacle_pcl->at(obs_it).y);
        float sudut_robot_ke_obs = angleToTarget(obstacle_pcl->at(obs_it).x, obstacle_pcl->at(obs_it).y, final_pose->x, final_pose->y);

        float delta_sudut_ = sudut_robot_ke_tujuan - sudut_robot_ke_obs;

        (delta_sudut_ > 180) ? delta_sudut_ -= 360 : (delta_sudut_ < -180) ? delta_sudut_ += 360
                                                                           : delta_sudut_;

        if (jarak_robot_ke_obs < min_dist && jarak_robot_ke_obs < 200 && fabs(delta_sudut_) < 45) {
            min_dist = jarak_robot_ke_obs;
            x_nearest_obs = obstacle_pcl->at(obs_it).x;
            y_nearest_obs = obstacle_pcl->at(obs_it).y;
            obs_ditemukan = 1;
        }
    }

    if (!obs_ditemukan) {
        return motionToPoint(target_x, target_y, target_theta, minmax_vel_linear, minmax_vel_angular);
    }

    //==================================================

    float sudut_obs_ke_robot = angleToTarget(final_pose->x, final_pose->y, x_nearest_obs, y_nearest_obs);
    float sudut_obs_ke_tujuan = angleToTarget(target_x, target_y, x_nearest_obs, y_nearest_obs);

    float sudut_selisih = sudut_obs_ke_tujuan - sudut_obs_ke_robot;

    (sudut_selisih > 180) ? sudut_selisih -= 360 : (sudut_selisih < -180) ? sudut_selisih += 360
                                                                          : sudut_selisih;

    float sudut_menghindar = sudut_obs_ke_robot + sudut_selisih * 0.5;
    float jarak_menghindar = 120;

    float target_x_baru = x_nearest_obs + jarak_menghindar * cos(sudut_menghindar DEG2RAD);
    float target_y_baru = y_nearest_obs + jarak_menghindar * sin(sudut_menghindar DEG2RAD);

    //==================================================

    return motionToPoint(target_x_baru, target_y_baru, target_theta, minmax_vel_linear, minmax_vel_angular);
}

int8_t Motion::motionToPointAvoidance(float target_x, float target_y, float target_theta, uint16_t minmax_vel_linear, uint16_t minmax_vel_angular, uint8_t flags)
{
    // TODO : Make Config
    static PID linear_pid(1.8, 0, 0);
    static PID angular_pid(1.8, 0, 0.5);

#ifndef POTENTIAL_ATTR
#define POTENTIAL_ATTR(r, kp) (0.5 * kp * r * r)
#endif

#ifndef POTENTIAL_REPL
#define POTENTIAL_REPL(rho, rho_max, eta) ((rho <= rho_max) ? (0.5 * eta * ((1.0f / rho) - (1.0f / rho_max)) * ((1.0f / rho) - (1.0f / rho_max))) : (0))
#endif

    /* Create virtual points */
    int virtual_points[2048];
    uint16_t idx = 0;
    for (int x = final_pose->x - 250; x <= final_pose->x + 250; x += 20) {
        for (int y = final_pose->y - 250; y <= final_pose->y + 250; y += 20) {
            virtual_points[idx] = x * !(idx & 1) + y * (idx & 1);
            idx++;
            virtual_points[idx] = x * !(idx & 1) + y * (idx & 1);
            idx++;
        }
    }

    /* Preparation */
    static int16_t min_index_gain = 0;
    float prev_gain = __FLT_MAX__;

    float min_obs_dist = __FLT_MAX__;
    float x_nearest_obs = 0;
    float y_nearest_obs = 1;

    /* Iterate per virtual points */
    for (int i = 0; i < idx / 2; i++) {
        int point_x = virtual_points[i << 1];
        int point_y = virtual_points[(i << 1) + 1];

        /* Create a valley */
        float dist_target = sqrtf((target_x - point_x) * (target_x - point_x) + (target_y - point_y) * (target_y - point_y));
        float pot_target = POTENTIAL_ATTR(dist_target, 0.003 * (53.f / 255.f));

        /* Create hill */
        float angle_to_point = atan2(point_y - final_pose->y, point_x - final_pose->x) RAD2DEG;
        float dist_point_to_robot = sqrtf((final_pose->x - point_x) * (final_pose->x - point_x) + (final_pose->y - point_y) * (final_pose->y - point_y));
        float pot_obs_rad = 0;
        uint8_t obs_blocked = 1;

        /*Hill Obstacle*/
        for (size_t obs_it = 0; obs_it < obstacle->size(); obs_it++) {
            float dist_obstacle = pythagoras(center_cam[0], center_cam[1], obstacle->at(obs_it).x, obstacle->at(obs_it).y);
            float dist_point_to_obs = sqrtf((obstacle->at(obs_it).x - point_x) * (obstacle->at(obs_it).x - point_x) + (obstacle->at(obs_it).y - point_y) * (obstacle->at(obs_it).y - point_y));

            if (dist_point_to_obs != 0)
                pot_obs_rad += POTENTIAL_REPL(dist_point_to_obs, (float)(150), 1000000.f * (8.f / 255.f));

            if (deltaAngle(angle_to_point, obs_it * 2.5) < 90 && dist_point_to_obs < dist_point_to_robot) {
                if (dist_obstacle < min_obs_dist && dist_obstacle < 150) {
                    min_obs_dist = dist_obstacle;
                    x_nearest_obs = obstacle->at(obs_it).x;
                    y_nearest_obs = obstacle->at(obs_it).y;
                }
                obs_blocked = 255;
            }
        }

        /*Valley Ball*/
        if ((flags & normal_obstacle_ball) >> 0x02 && ball->status > 0) {
            float angle_robot_to_obs = angleToTarget(ball->pos.x, ball->pos.y);
            float dist_point_to_obs = sqrtf((ball->pos.x - point_x) * (ball->pos.x - point_x) + (ball->pos.y - point_y) * (ball->pos.y - point_y));

            if (dist_point_to_obs != 0)
                pot_obs_rad += POTENTIAL_REPL(dist_point_to_obs, (float)(400), 1000000.f * (9.f / 255.f));

            if (deltaAngle(angle_to_point, angle_robot_to_obs) < 90 && dist_point_to_obs < dist_point_to_robot)
                obs_blocked = 255;
        }

        // TODO : potential field from friend
        //  if ((flags & normal_obstacle_friend) >> 0x02) {
        //      for (uint8_t friend_it = 1; friend_it <= 5; friend_it++) {
        //          if (robot_data[friend_it].state == 1 && friend_it != atoi(getenv("ROBOT"))) {
        //              float pos_x_obs = robot_data[friend_it].pos_x;
        //              float pos_y_obs = robot_data[friend_it].pos_y;
        //              float angle_robot_2_obs = RobotAngletoPoint(pos_x_obs, pos_y_obs);

        //             float point_to_obs_dist = sqrtff((pos_x_obs - point_x) * (pos_x_obs - point_x) + (pos_y_obs - point_y) * (pos_y_obs - point_y));

        //             if (point_to_obs_dist != 0)
        //                 pot_obs_rad += POTENTIAL_REPL(point_to_obs_dist, (float)(400), 1000000.f * (9.f / 255.f));

        //             if (delta_angle(angle_to_point, angle_robot_2_obs) < 90 && point_to_obs_dist < distance_to_point)
        //                 obs_blocked = 255;
        //         }
        //     }
        // }

        /* Finding lowest artificial potential */
        float final_gain = pot_target + pot_obs_rad;
        final_gain *= obs_blocked;

        if (final_gain < prev_gain) {
            min_index_gain = i;
            prev_gain = final_gain;
        }
    }

    float error_x = 0;
    float error_y = 0;
    float error_linear = 0;
    float error_angular = 0;

    error_x = virtual_points[min_index_gain << 1] - final_pose->x;
    error_y = virtual_points[(min_index_gain << 1) + 1] - final_pose->y;

    error_linear = sqrtf(error_x * error_x + error_y * error_y);
    error_angular = target_theta - final_pose->theta;

    if ((flags & invert_obs) >> 0x04 && min_obs_dist != __FLT_MAX__) {
        error_angular = deltaAngle(atan2(final_pose->y - y_nearest_obs, final_pose->x - x_nearest_obs) RAD2DEG, final_pose->y, 0);
    }

    (error_angular > 180) ? error_angular -= 360 : (error_angular < -180) ? error_angular += 360
                                                                          : error_angular;

    vel2d_t output_vel;
    float buffer_linear_vel;
    buffer_linear_vel = linear_pid.calculate(error_linear, minmax_vel_linear);
    output_vel.theta = angular_pid.calculate(error_angular, minmax_vel_angular);

    output_vel.x = buffer_linear_vel * cosf(atan2(error_y, error_x));
    output_vel.y = buffer_linear_vel * sinf(atan2(error_y, error_x));

    worldToLocalMotion(output_vel.x, output_vel.y, output_vel.theta);

    return (pythagoras(final_pose->x, final_pose->y, target_x, target_y) < 30 && fabs(error_angular) < 5 && pythagoras(final_pose->x, final_pose->y, virtual_points[min_index_gain << 1], virtual_points[(min_index_gain << 1) + 1]) < 60);
}

int8_t Motion::motionAroundPointAngle(float target_x, float target_y, float target_arc_theta, float robot_target_th, uint16_t radius_point, uint16_t minmax_vel_linear, uint16_t minmax_vel_angular, uint8_t flag)
{
    // static PID linear_pid(1.2, 0, 0);
    // static PID angular_pid(3, 0, 0);
    // static PID arching_pid(0.9, 0, 0); // 2.1 // 1.3 0..0000001 0 // 2,0,0 //1.9 //1.4  //1.5

    static PID linear_pid(1.5, 0, 0);
    static PID angular_pid(3, 0, 0);
    static PID arching_pid(1.1, 0, 0); // 2.1 // 1.3 0..0000001 0 // 2,0,0 //1.9 //1.4  //1.5

    float error_x = 0;
    float error_y = 0;
    float error_linear = 0;
    float error_angular = 0;

    error_x = target_x - final_pose->x;
    error_y = target_y - final_pose->y;

    error_linear = sqrtf(error_x * error_x + error_y * error_y) - radius_point;
    error_angular = robot_target_th - final_pose->theta - 180 * ((flag & INVERT) == INVERT);

    (error_angular > 180) ? error_angular -= 360 : (error_angular < -180) ? error_angular += 360
                                                                          : error_angular;

    float error_arching = target_arc_theta - angleToTarget(target_x, target_y);
    (error_arching > 180) ? error_arching -= 360 : (error_arching < -180) ? error_arching += 360
                                                                          : error_arching;

    float circumfence = 2 * M_PI * pythagoras(target_x, final_pose->x, target_y, final_pose->y);
    float arc_error = circumfence * error_arching * 0.00277777777;

    /* Output Position */
    float output_linear = linear_pid.calculate(error_linear, minmax_vel_linear);
    /* Output Theta */
    float output_angular = angular_pid.calculate(error_angular, minmax_vel_angular);
    /* Output Arc */
    float output_arching = arching_pid.calculate(arc_error, minmax_vel_linear);

    vel2d_t output_vel;

    output_vel.x = output_linear * cosf(atan2(error_y, error_x)) + output_arching * cosf(atan2(error_y, error_x) - M_PI_2);
    output_vel.y = output_linear * sinf(atan2(error_y, error_x)) + output_arching * sinf(atan2(error_y, error_x) - M_PI_2);
    output_vel.theta = output_angular;

    // printf("Error Linear : %.1f Error Angular : %.1f Error Arching : %.1f\n", error_linear, error_angular, error_arching);
    // printf("Output Linear : %.1f Output Angular : %.1f Output Arching : %.1f\n", output_linear, output_angular, output_arching);
    // printf("Output X : %.1f Output Y : %.1f Output Theta : %.1f\n", output_vel.x, output_vel.y, output_vel.theta);

    float output_x = output_vel.x;
    float output_y = output_vel.y;

    if ((flag & NORMAL_OBSTACLE) == NORMAL_OBSTACLE && (flag & NORMAL_OBSTACLE_BALL) == NORMAL_OBSTACLE_BALL) {
        output_x = ObstacleAvoidance(output_vel.x, output_vel.y, 90, 120, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL).vx;
        output_y = ObstacleAvoidance(output_vel.x, output_vel.y, 90, 120, NORMAL_OBSTACLE | NORMAL_OBSTACLE_BALL).vy;
    } else if ((flag & NORMAL_OBSTACLE) == NORMAL_OBSTACLE) {
        output_x = ObstacleAvoidance(output_vel.x, output_vel.y, 90, 100, NORMAL_OBSTACLE).vx;
        output_y = ObstacleAvoidance(output_vel.x, output_vel.y, 90, 100, NORMAL_OBSTACLE).vy;
    } else if ((flag & NORMAL_OBSTACLE_BALL) == NORMAL_OBSTACLE_BALL) {
        output_x = ObstacleAvoidance(output_vel.x, output_vel.y, 90, 100, NORMAL_OBSTACLE_BALL).vx;
        output_y = ObstacleAvoidance(output_vel.x, output_vel.y, 90, 100, NORMAL_OBSTACLE_BALL).vy;
    }

    worldToLocalMotion(output_x, output_y, output_vel.theta);

    if (fabs(error_linear) < 5 && fabs(error_angular) < 9 && fabs(error_arching) < 9) {
        return 1;
    } else {
        return 0;
    }
}

void Motion::worldToLocalMotion(float vel_x, float vel_y, float vel_theta, bool is_ruckig)
{
    world_vel->x = vel_x;
    world_vel->y = vel_y;
    world_vel->theta = vel_theta;

    vel2d_t buffer_vel;
    buffer_vel.x = vel_x * sinf(final_pose->theta DEG2RAD) - vel_y * cosf(final_pose->theta DEG2RAD);
    buffer_vel.y = vel_x * cosf(final_pose->theta DEG2RAD) + vel_y * sinf(final_pose->theta DEG2RAD);

    // printf("world : %.f %.f %.f\n", buffer_vel.x, buffer_vel.y, final_pose->theta);

    manualMotion(buffer_vel.x, buffer_vel.y, vel_theta, is_ruckig);
    return;
}

float Motion::angleToTarget(float target_x, float target_y)
{
    float dx = target_x - final_pose->x;
    float dy = target_y - final_pose->y;

    return atan2(dy, dx) RAD2DEG;
}

float Motion::angleToTarget(float target_x, float target_y, float base_x, float base_y)
{
    float dx = target_x - base_x;
    float dy = target_y - base_y;

    return atan2(dy, dx) RAD2DEG;
}

uint8_t Motion::receiveBall(float vel_linear, float vel_angular)
{
    static MachineState recv_ball_state;
    static float last_ball_pos[2] = { 0, 0 };
    static float delta_ball_pos[2] = { 0, 0 };
    static float ball_angle = 0;
    static float ball_distance = 0;
    static float blocking_distance = 0;
    static float ball_angle_to_robot = 0;
    static float delta_error_angle = 0;
    static uint8_t counter = 0;

    static uint8_t counter_bola_salah;

    recv_ball_state.reentry(0, 0.03);
    printf("RECV State : %d Status : %d || %.2f %.2f\n", recv_ball_state.value, ball->status, fabs(delta_error_angle), ball_distance);
    // printf("Angle : %.1f Robot Angle : %.1f\n", ball_angle, ball_angle_to_robot);
    // printf("Distance : %.1f Blocking : %.1f\n", ball_distance, blocking_distance);
    // printf("Robot to ball : %.1f\n", pythagoras(final_pose->x, final_pose->y, ball->pos.x, ball->pos.y));
    switch (recv_ball_state.value) {
    case 0:
        if (ball->status == 2 || ball->status == 1) {
            last_ball_pos[0] = ball->pos.x;
            last_ball_pos[1] = ball->pos.y;
            recv_ball_state.value = 1;
        }

        if (ball->is_caught > 0) {
            recv_ball_state.value = 2;
        }

        counter_bola_salah = 0;
        manualMotion(0, 0, 0);
        break;
    case 1:
        if (ball->status == 0) {
            recv_ball_state.value = 0;
        }

        delta_ball_pos[0] = ball->pos.x - last_ball_pos[0];
        delta_ball_pos[1] = ball->pos.y - last_ball_pos[1];
        ball_distance = pythagoras(ball->pos.x, ball->pos.y, last_ball_pos[0], last_ball_pos[1]);
        blocking_distance = pythagoras(final_pose->x, final_pose->y, last_ball_pos[0], last_ball_pos[1]);
        ball_angle = angleToTarget(ball->pos.x, ball->pos.y, last_ball_pos[0], last_ball_pos[1]);
        ball_angle_to_robot = angleToTarget(final_pose->x, final_pose->y, ball->pos.x, ball->pos.y);
        delta_error_angle = ball_angle - ball_angle_to_robot;

        stm_data->dribble_up = 1;

        if (delta_error_angle > 180)
            delta_error_angle -= 360;
        else if (delta_error_angle < -180)
            delta_error_angle += 360;

        if (pythagoras(final_pose->x, final_pose->y, ball->pos.x, ball->pos.y) < 80) {
            printf("pythagoras(final_pose->x, final_pose->y, ball->pos.x, ball->pos.y) < 60\n");
            recv_ball_state.value = 2;
        } else if (fabs(delta_error_angle) < 30 && ball_distance > 50) {
            printf("fabs(delta_error_angle) < 30 && ball_distance > 40\n");

            motionAroundPointAngle(last_ball_pos[0], last_ball_pos[1], ball_angle - 180, (atan2(ball->pos.y - last_ball_pos[1], ball->pos.x - last_ball_pos[0]) RAD2DEG) - 180, blocking_distance, 500, 360);
        } else {
            printf("else\n");

            if (counter_bola_salah > 1 && ball->status == 2) {
                recv_ball_state.value = 0;
                break;
            }

            if (fabs(delta_error_angle) > 30) {
                counter_bola_salah++;
            }

            if (ball->status > 0) {
                motionToPoint(final_pose->x, final_pose->y, ball_angle_to_robot - 180, 500, 3);
            } else {
                manualMotion(0, 0, 0);
            }
        }

        if (ball->is_caught > 0) {
            recv_ball_state.value = 2;
        }
        break;
    case 2:
        if (catchingBall(vel_linear, vel_angular)) {
            stm_data->dribble_up = 0;
            recv_ball_state.value = 3;
        }
        break;
    default:
        break;
    }

    if (recv_ball_state.value == 3)
        return 1;
    else
        return 0;
}

uint8_t Motion::receiveBall2(float default_x, float default_y, float vel_linear, float vel_angular)
{
    static MachineState recv_ball_state;
    static float last_ball_pos[2] = { 0, 0 };
    static float delta_ball_pos[2] = { 0, 0 };
    static float ball_angle = 0;
    static float ball_distance = 0;
    static float blocking_distance = 0;
    static float ball_angle_to_robot = 0;
    static float delta_error_angle = 0;
    static uint8_t counter = 0;

    static uint8_t counter_bola_salah = 0;

    recv_ball_state.reentry(0, 0.04);
    // printf("State : %d Status : %d\n", recv_ball_state.value, ball->status);
    // printf("Angle : %.1f Robot Angle : %.1f\n", ball_angle, ball_angle_to_robot);
    // printf("Distance : %.1f Blocking : %.1f\n", ball_distance, blocking_distance);
    // printf("Robot to ball : %.1f\n", pythagoras(final_pose->x, final_pose->y, ball->pos.x, ball->pos.y));
    switch (recv_ball_state.value) {
    case 0:
        printf("case  0\n");
        if (ball->status == 2) {
            last_ball_pos[0] = ball->pos.x;
            last_ball_pos[1] = ball->pos.y;
            recv_ball_state.value = 1;
        }

        counter_bola_salah = 0;
        motionToPoint(final_pose->x, final_pose->y, angleToTarget(default_x, default_y), 0, 90);
        break;
    case 1:
        printf("case  1\n");

        if (ball->status != 2) {
            recv_ball_state.value = 0;
            break;
        }

        delta_ball_pos[0] = ball->pos.x - last_ball_pos[0];
        delta_ball_pos[1] = ball->pos.y - last_ball_pos[1];
        ball_distance = pythagoras(ball->pos.x, ball->pos.y, last_ball_pos[0], last_ball_pos[1]);
        blocking_distance = pythagoras(final_pose->x, final_pose->y, last_ball_pos[0], last_ball_pos[1]);
        ball_angle = angleToTarget(ball->pos.x, ball->pos.y, last_ball_pos[0], last_ball_pos[1]);
        ball_angle_to_robot = angleToTarget(final_pose->x, final_pose->y, ball->pos.x, ball->pos.y);
        delta_error_angle = ball_angle - ball_angle_to_robot;

        stm_data->dribble_up = 1;

        if (delta_error_angle > 180)
            delta_error_angle -= 360;
        else if (delta_error_angle < -180)
            delta_error_angle += 360;

        if (pythagoras(final_pose->x, final_pose->y, ball->pos.x, ball->pos.y) < 90) {
            printf("pythagoras(final_pose->x, final_pose->y, ball->pos.x, ball->pos.y) < 60\n");
            recv_ball_state.value = 2;
        } else if (fabs(delta_error_angle) < 45 && ball_distance > 35) {
            printf("fabs(delta_error_angle) < 45 && ball_distance > 35\n");
            motionAroundPointAngle(last_ball_pos[0], last_ball_pos[1], ball_angle - 180, ball_angle_to_robot - 180, blocking_distance, 500, 360);
        } else {
            printf("else\n");
            if (counter_bola_salah > 3 && ball->status == 2) {
                recv_ball_state.value = 0;
                break;
            }
            motionToPoint(final_pose->x, final_pose->y, ball_angle_to_robot - 180, 500, 360);
            counter_bola_salah++;
        }
        break;
    case 2:
        printf("case  2\n");

        if (catchingBall(vel_linear, vel_angular)) {
            stm_data->dribble_up = 0;
            recv_ball_state.value = 3;
        }
        break;
    default:
        break;
    }

    if (recv_ball_state.value == 3)
        return 1;
    else
        return 0;
}

uint8_t Motion::receiveBall3(float default_x, float default_y, float vel_linear, float vel_angular)
{
    static MachineState recv_ball_state;
    static float arah_bola;
    static float sudut_ke_bola;
    static float delta_theta;
    static float waypoint_terdekat_x;
    static float waypoint_terdekat_y;
    static float jarak_waypoint_terdekat;

    static uint16_t cnt_ = 0;

    recv_ball_state.reentry(0, 0.04);
    printf("%d |status : %d predict : %.2f %.2f ball_status : %d size wp : %d\n", stm_data->stand_by, recv_ball_state.value, waypoint_terdekat_x, waypoint_terdekat_y, ball->status, waypoints_ball->size());
    switch (recv_ball_state.value) {
    case 0:
        stm_data->stand_by = 1;

        if (ball->is_caught > 0) {
            recv_ball_state.value = 2;
            break;
        }

        if (ball->status > 0) {
            if (pythagoras(final_pose->x, final_pose->y, ball->pos.x, ball->pos.y) < 70) {
                recv_ball_state.value = 2;
                break;
            }
        }

        use_acceleration_constant = 0;
        jarak_waypoint_terdekat = FLT_MAX;
        if (waypoints_ball->size() > 1 && ball->status > 0) {
            sudut_ke_bola = angleToTarget(waypoints_ball->at(0).x, waypoints_ball->at(0).y);
            arah_bola = atan2(ball->vel.y, ball->vel.x) RAD2DEG;
            delta_theta = deltaAngle(sudut_ke_bola, arah_bola - 180);
            if (arah_bola > 180) {
                arah_bola -= 360;
            } else if (arah_bola < -180) {
                arah_bola += 360;
            }
            if (delta_theta > 180) {
                delta_theta -= 360;
            } else if (delta_theta < -180) {
                delta_theta += 360;
            }
            if (fabs(delta_theta) < 30) {
                recv_ball_state.value = 1;
            }
        }
        if (ball->status == 0) {
            manualMotion(0, 0, 0);
        } else {
            motionToPointRecvBall(final_pose->x, final_pose->y, sudut_ke_bola, 0, 3);
        }

        cnt_ = 0;

        break;
    case 1:
        stm_data->stand_by = 1;

        if (ball->is_caught > 0) {
            recv_ball_state.value = 2;
            break;
        }

        if (pythagoras(final_pose->x, final_pose->y, ball->pos.x, ball->pos.y) < 70) {
            stm_data->stand_by = 0;
            recv_ball_state.value = 2;
            break;
        }

        if (waypoints_ball->size() == 1) {
            recv_ball_state.value = 0;
            break;
        }

        jarak_waypoint_terdekat = FLT_MAX;
        for (int i = 0; i < waypoints_ball->size(); i++) {
            if (pythagoras(waypoints_ball->at(i).x, waypoints_ball->at(i).y, final_pose->x, final_pose->y) < jarak_waypoint_terdekat) {
                jarak_waypoint_terdekat = pythagoras(waypoints_ball->at(i).x, waypoints_ball->at(i).y, final_pose->x, final_pose->y);
                waypoint_terdekat_x = waypoints_ball->at(i).x;
                waypoint_terdekat_y = waypoints_ball->at(i).y;
            }
        }
        sudut_ke_bola = angleToTarget(waypoints_ball->at(0).x, waypoints_ball->at(0).y);
        arah_bola = atan2(waypoints_ball->at(1).y - waypoints_ball->at(0).y, waypoints_ball->at(1).x - waypoints_ball->at(0).x) RAD2DEG;
        if (arah_bola > 180) {
            arah_bola -= 360;
        } else if (arah_bola < -180) {
            arah_bola += 360;
        }

        target_intercept_ball_x = waypoint_terdekat_x;
        target_intercept_ball_y = waypoint_terdekat_y;

        static const float kecepatan_bola_gain = 1;

        if (pythagoras(final_pose->x, final_pose->y, target_intercept_ball_x, target_intercept_ball_y) > 300) {
            // motionToPointRecvBall(target_intercept_ball_x, target_intercept_ball_y, sudut_ke_bola, 70, 10, 1); // 50,90 27 mei
            motionToPointRecvBall(target_intercept_ball_x, target_intercept_ball_y, sudut_ke_bola, 10, 10, 1, (waypoints_ball->at(1).x - waypoints_ball->at(0).x) * kecepatan_bola_gain, (waypoints_ball->at(1).y - waypoints_ball->at(0).y) * kecepatan_bola_gain); // 50,90
        } else {
            // motionToPointRecvBall(target_intercept_ball_x, target_intercept_ball_y, arah_bola - 180, 200, 90, 1);
            motionToPointRecvBall(target_intercept_ball_x, target_intercept_ball_y, ball->angle, 200, 90, 1, (waypoints_ball->at(1).x - waypoints_ball->at(0).x) * kecepatan_bola_gain, (waypoints_ball->at(1).y - waypoints_ball->at(0).y) * kecepatan_bola_gain);
        }
        // motionToPointRecvBall(target_intercept_ball_x, target_intercept_ball_y, arah_bola - 180, 200, 180, 1);

        cnt_ = 0;

        break;

    case 2:
        stm_data->stand_by = 1;

        use_acceleration_constant = 0;
        if (catchingBall(vel_linear, vel_angular)) {
            recv_ball_state.value = 3;
        }
        break;
    }

    if (recv_ball_state.value == 3) {
        // stm_data->stand_by = 0;
        printf("RECV EXIT\n");
        stm_data->force_dribble_control = 1;
        return 1;
    } else {
        stm_data->stand_by = 1;
        stm_data->force_dribble_control = 2;
        return 0;
    }
}

bool Motion::passBall(float target_x, float target_y, float kick_power, float tolerance)
{
    static MachineState pass_ball_state;
    pass_ball_state.reentry(0, 0.05);

    switch (pass_ball_state.value) {
    case 0:
        if (ball->status > 0) {
            pass_ball_state.value = 1;
        } else {
            catchingBall(100, 80);
        }
        break;
    case 1:
        // if (PivotMotion(RobotAngletoPoint(robot_data[target].pos_x, robot_data[target].pos_y), 200))
        //     pass_ball_state.value = 2;
        if (motionToPoint(final_pose->x, final_pose->y, angleToTarget(target_x, target_y), 50, 180))
            pass_ball_state.value = 2;

        if (deltaAngle(final_pose->theta, angleToTarget(target_x, target_y)) < tolerance)
            pass_ball_state.value = 2;

        if (ball->is_caught == 0) //*?
            pass_ball_state.value = 0;
        break;
    case 2:
        manualMotion(0, 0, 0);
        if (ball->is_caught == 0) {
            pass_ball_state.value = 3;
        } else {
            kickerControl(stm_data, NO_AIM, kick_power, 900);
        }
        break;
    }
    if (pass_ball_state.value == 3)
        return true;
    else
        return false;
}

uint8_t Motion::catchingBall(float vel_linear, float vel_angular, uint8_t flag)
{
    stm_data->dribble_up = 1;
    static PID angle_pid(2.5, 0, 0.01);

    static MachineState catch_ball_state;
    catch_ball_state.reentry(0, 0.04);

    float error_angle = ball->angle - final_pose->theta;

    while (error_angle > 180)
        error_angle -= 360;
    while (error_angle < -180)
        error_angle += 360;

    float output_angle = angle_pid.calculate(error_angle, vel_angular);

    static float target_ball_x;
    static float target_ball_y;

    target_ball_x = ball->pos.x;
    target_ball_y = ball->pos.y;

    float minimum_distance = FLT_MAX;

    for (size_t i = 0; i < waypoints_ball->size(); i++) {
        if (pythagoras(final_pose->x, final_pose->y, waypoints_ball->at(i).x, waypoints_ball->at(i).y) < minimum_distance) {
            minimum_distance = pythagoras(final_pose->x, final_pose->y, waypoints_ball->at(i).x, waypoints_ball->at(i).y);
            target_ball_x = waypoints_ball->at(i).x;
            target_ball_y = waypoints_ball->at(i).y;
        }
    }

    static float counter_tambah_kecepatan = 0;

    obstacle_checking_t obs_depan = obstacleChecking(final_pose->theta, 25, 70, NORMAL_OBSTACLE);

    printf("KEJARBOLA: %d %d %d || %.2f %.2f\n", catch_ball_state.value, ball->status, ball->is_caught, ball->distance, fabs(error_angle));

    switch (catch_ball_state.value) {
    case 0:
        counter_tambah_kecepatan = 0;
        if (ball->is_caught > 0) {
            manualMotion(0, 0, 0);
            break;
        }

        if (ball->distance < 100 && fabs(error_angle) < 10 && ball->status > 0) {
            catch_ball_state.value = 1;
        } else if (waypoints_ball->size() > 1 && ball->status > 0) {
            catch_ball_state.value = 2;
        } else if (ball->status > 0) {
            catch_ball_state.value = 3;
        }

        manualMotion(0, 0, 0);
        break;
    case 1:
        if (ball->distance < 100 && fabs(error_angle) > 20 && ball->status > 0) {
            catch_ball_state.value = 3;
        } else if (ball->distance < 100 && fabs(error_angle) > 20 && ball->status == 0) {
            catch_ball_state.value = 0;
        }

        if (obs_depan.status) {
            counter_tambah_kecepatan += 1.5;
            manualMotion(0, vel_linear + counter_tambah_kecepatan, output_angle);
        } else {
            manualMotion(0, vel_linear, output_angle);
        }
        break;
    case 2:
        counter_tambah_kecepatan = 0;
        if (ball->status == 0 || ball->is_caught > 0) {
            catch_ball_state.value = 0;
            break;
        }

        if (ball->distance < 120 && fabs(error_angle) < 7.5 && ball->status > 0) {
            catch_ball_state.value = 1;
        } else if (waypoints_ball->size() <= 1) {
            catch_ball_state.value = 3;
        }

        motionToPoint(target_ball_x, target_ball_y, angleToTarget(ball->pos.x, ball->pos.y), vel_linear, vel_angular, flag);
        break;
    case 3:
        counter_tambah_kecepatan = 0;
        if (ball->status == 0 || ball->is_caught > 0) {
            catch_ball_state.value = 0;
            break;
        }

        if (ball->distance < 120 && fabs(error_angle) < 7.5 && ball->status > 0) {
            catch_ball_state.value = 1;
        } else if (waypoints_ball->size() > 1) {
            catch_ball_state.value = 2;
        }

        motionToPoint(ball->pos.x, ball->pos.y, angleToTarget(ball->pos.x, ball->pos.y), vel_linear, vel_angular, flag);
        break;
    }

    static int counter = 0;
    if (ball->is_caught > 0) {
        counter++;
    } else {
        counter = 0;
    }

    if (counter > 2) {
        manualMotion(0, 0, 0);
    }

    if ((!obs_depan.status && counter > 4)) {
        printf("TIDAK ADA OBS\n");
        stm_data->force_dribble_control = 1;
        return 1;
    } else if (obs_depan.status && counter > 10) {
        return 1;
        stm_data->force_dribble_control = 2;
    } else {
        if ((flag & LANGSUNG_AMBIL) == LANGSUNG_AMBIL) {
            stm_data->force_dribble_control = 4;
        } else {
            if (ball->status > 0 && ball->distance < 120) {
                stm_data->force_dribble_control = 2;
                // stm_data->force_dribble_control = 1;
            } else {
                stm_data->force_dribble_control = 2;
                // stm_data->force_dribble_control = 1;
            }
        }

        return 0;
    }
}

obstacle_checking_t Motion::obstacleChecking(float theta, float tolerance, float dist, uint8_t flags)
{
    obstacle_checking_t ret_temp;

    static float last_obs_x = 0;
    static float last_obs_y = 0;

    pose_used->x = final_pose->x;
    pose_used->y = final_pose->y;

    static const float scaler = 0.0005;
    float translation_vel = sqrt(world_vel->x * world_vel->x + world_vel->y * world_vel->y) * scaler;
    float magnitude_robot_to_obs = sqrt((final_pose->x - last_obs_x) * (final_pose->x - last_obs_x) + (final_pose->y - last_obs_y) * (final_pose->y - last_obs_y));
    float angle_robot_to_obs = angleToTarget(last_obs_x, last_obs_y, final_pose->x, final_pose->y);

    float new_magnitude = magnitude_robot_to_obs * translation_vel;
    float norm_magnitude = new_magnitude / magnitude_robot_to_obs;

    if (norm_magnitude > 1)
        norm_magnitude = 1;

    if ((flags & PREDICT) == PREDICT) {
        float act_vel_mag = sqrt(act_vel->x * act_vel->x + act_vel->y * act_vel->y);
        pose_used->x = final_pose->x + 0 * cosf(theta DEG2RAD);
        pose_used->y = final_pose->y + 0 * sinf(theta DEG2RAD);

        dist += act_vel_mag * 0.02;
        // pose_used->x = final_pose->x + magnitude_robot_to_obs * norm_magnitude * cosf(angle_robot_to_obs DEG2RAD);
        // pose_used->y = final_pose->y + magnitude_robot_to_obs * norm_magnitude * sinf(angle_robot_to_obs DEG2RAD);
    }
    // printf("PREDICT: %.2f %.2f | %.2f %.2f, | %.2f %.2f | %.2f | %.2f %.2f || %.2f\n", pose_used->x, pose_used->y, final_pose->x, final_pose->y, norm_magnitude, magnitude_robot_to_obs, angle_robot_to_obs, last_obs_x, last_obs_y, dist);

    memset(&ret_temp, 0, sizeof(obstacle_checking_t));

    float min_dist = FLT_MAX;

    // Menggunakan obstacle yang sudah digabung
    if ((flags & USE_OBS_MERGE) == USE_OBS_MERGE) {
        if ((flags & NORMAL_OBSTACLE) == NORMAL_OBSTACLE) {
            for (size_t obs_it = 0; obs_it < obstacle->size(); obs_it++) {
                float jarak_robot_ke_obs = pythagoras(pose_used->x, pose_used->y, obstacle->at(obs_it).x, obstacle->at(obs_it).y);
                float sudut_robot_ke_obs = angleToTarget(obstacle->at(obs_it).x, obstacle->at(obs_it).y, pose_used->x, pose_used->y);

                float delta_sudut_ = theta - sudut_robot_ke_obs;

                while (delta_sudut_ > 180)
                    delta_sudut_ -= 360;
                while (delta_sudut_ < -180)
                    delta_sudut_ += 360;

                if (jarak_robot_ke_obs < min_dist && jarak_robot_ke_obs < dist && fabs(delta_sudut_) < tolerance) {
                    ret_temp.status = 1;
                    ret_temp.distance = jarak_robot_ke_obs;
                    ret_temp.angle = sudut_robot_ke_obs;
                    ret_temp.pos_x = obstacle->at(obs_it).x;
                    ret_temp.pos_y = obstacle->at(obs_it).y;

                    last_obs_x = ret_temp.pos_x;
                    last_obs_y = ret_temp.pos_y;

                    // printf("RET1: %.2f %.2f\n", last_obs_x, last_obs_y);

                    min_dist = jarak_robot_ke_obs;
                }
            }
        }

        if ((flags & NORMAL_OBSTACLE_BALL) == NORMAL_OBSTACLE_BALL) {
            if (ball->status > 0) {
                float sudut_robot_ke_bola = angleToTarget(ball->pos.x, ball->pos.y);
                float delta_sudut_bola = theta - sudut_robot_ke_bola;

                while (delta_sudut_bola > 180)
                    delta_sudut_bola -= 360;
                while (delta_sudut_bola < -180)
                    delta_sudut_bola += 360;

                if (ball->distance < dist && fabs(delta_sudut_bola) < tolerance) {
                    if (ret_temp.status == 1 && ball->distance < ret_temp.distance) {
                        ret_temp.distance = ball->distance;
                        ret_temp.angle = sudut_robot_ke_bola;
                        ret_temp.pos_x = ball->pos.x;
                        ret_temp.pos_y = ball->pos.y;
                        ret_temp.status = 1;

                        last_obs_x = ret_temp.pos_x;
                        last_obs_y = ret_temp.pos_y;

                        // printf("RET2: %.2f %.2f\n", last_obs_x, last_obs_y);
                    } else if (ret_temp.status == 0) {
                        ret_temp.distance = ball->distance;
                        ret_temp.angle = sudut_robot_ke_bola;
                        ret_temp.pos_x = ball->pos.x;
                        ret_temp.pos_y = ball->pos.y;
                        ret_temp.status = 1;

                        last_obs_x = ret_temp.pos_x;
                        last_obs_y = ret_temp.pos_y;

                        // printf("RET3: %.2f %.2f\n", last_obs_x, last_obs_y);
                    }
                }
            }
        }

        obstacle_checking_t ret_temp_temp;
        memcpy(&ret_temp_temp, &ret_temp, sizeof(obstacle_checking_t));

        if ((flags & NORMAL_OBSTACLE_FRIEND) == NORMAL_OBSTACLE_FRIEND) {
            float min_dist_friend = FLT_MAX;
            for (uint8_t i = 1; i < 4; i++) {
                float dist_to_friend = RobotDist2Point(shared_robot_data[i]->pose.x, shared_robot_data[i]->pose.y);
                float angle_to_friend = RobotAngletoPoint(shared_robot_data[i]->pose.x, shared_robot_data[i]->pose.y);

                // printf("Hello: %d %.2f %.2f || %.2f %.2f || %.2f %.2f\n", i, shared_robot_data[i]->pose.x, shared_robot_data[i]->pose.y, dist_to_friend, deltaAngle(angle_to_friend, theta), angle_to_friend, theta);

                // Skip ketika melihat obstacle lebih dekat
                if ((flags & NORMAL_OBSTACLE) == NORMAL_OBSTACLE || (flags & NORMAL_OBSTACLE_BALL) == NORMAL_OBSTACLE_BALL) {
                    if (ret_temp_temp.status == 1 && ret_temp_temp.distance < dist_to_friend) {
                        continue;
                    }
                }

                if (dist_to_friend < min_dist_friend && dist_to_friend < dist && deltaAngle(angle_to_friend, theta) < tolerance) {
                    min_dist_friend = dist_to_friend;
                    ret_temp.pos_x = shared_robot_data[i]->pose.x;
                    ret_temp.pos_y = shared_robot_data[i]->pose.y;
                    ret_temp.distance = dist_to_friend;
                    ret_temp.angle = angle_to_friend;
                    ret_temp.status = 1;

                    last_obs_x = ret_temp.pos_x;
                    last_obs_y = ret_temp.pos_y;

                    // printf("RET4: %.2f %.2f\n", last_obs_x, last_obs_y);
                }
            }
        }

        last_obs_x = ret_temp.pos_x;
        last_obs_y = ret_temp.pos_y;

        // printf("RET5: %.2f %.2f\n", last_obs_x, last_obs_y);

        return ret_temp;
    }

    // Mengunakan obstacle pcl
    if ((flags & NORMAL_OBSTACLE) == NORMAL_OBSTACLE) {
        for (size_t obs_it = 0; obs_it < obstacle_pcl->size(); obs_it++) {
            float jarak_robot_ke_obs = pythagoras(pose_used->x, pose_used->y, obstacle_pcl->at(obs_it).x, obstacle_pcl->at(obs_it).y);
            float sudut_robot_ke_obs = angleToTarget(obstacle_pcl->at(obs_it).x, obstacle_pcl->at(obs_it).y, pose_used->x, pose_used->y);

            float delta_sudut_ = theta - sudut_robot_ke_obs;

            (delta_sudut_ > 180) ? delta_sudut_ -= 360 : (delta_sudut_ < -180) ? delta_sudut_ += 360
                                                                               : delta_sudut_;

            if (jarak_robot_ke_obs < min_dist && jarak_robot_ke_obs < dist && fabs(delta_sudut_) < tolerance) {
                ret_temp.status = 1;
                ret_temp.distance = jarak_robot_ke_obs;
                ret_temp.angle = sudut_robot_ke_obs;
                ret_temp.pos_x = obstacle_pcl->at(obs_it).x;
                ret_temp.pos_y = obstacle_pcl->at(obs_it).y;

                last_obs_x = ret_temp.pos_x;
                last_obs_y = ret_temp.pos_y;

                // printf("RET6: %.2f %.2f\n", last_obs_x, last_obs_y);

                min_dist = jarak_robot_ke_obs;
            }
        }
    }

    if ((flags & NORMAL_OBSTACLE_BALL) == NORMAL_OBSTACLE_BALL) {
        if (ball->status > 0) {
            float sudut_robot_ke_bola = angleToTarget(ball->pos.x, ball->pos.y);
            float delta_sudut_bola = theta - sudut_robot_ke_bola;

            while (delta_sudut_bola > 180)
                delta_sudut_bola -= 360;
            while (delta_sudut_bola < -180)
                delta_sudut_bola += 360;

            if (ball->distance < dist && fabs(delta_sudut_bola) < tolerance) {
                if (ret_temp.status == 1 && ball->distance < ret_temp.distance) {
                    ret_temp.distance = ball->distance;
                    ret_temp.angle = sudut_robot_ke_bola;
                    ret_temp.pos_x = ball->pos.x;
                    ret_temp.pos_y = ball->pos.y;

                    last_obs_x = ret_temp.pos_x;
                    last_obs_y = ret_temp.pos_y;

                    // printf("RET7: %.2f %.2f\n", last_obs_x, last_obs_y);

                    ret_temp.status = 1;
                } else if (ret_temp.status == 0) {
                    ret_temp.distance = ball->distance;
                    ret_temp.angle = sudut_robot_ke_bola;
                    ret_temp.pos_x = ball->pos.x;
                    ret_temp.pos_y = ball->pos.y;

                    last_obs_x = ret_temp.pos_x;
                    last_obs_y = ret_temp.pos_y;

                    // printf("RET8 %.2f %.2f\n", last_obs_x, last_obs_y);

                    ret_temp.status = 1;
                }
            }
        }
    }

    obstacle_checking_t ret_temp_temp;
    memcpy(&ret_temp_temp, &ret_temp, sizeof(obstacle_checking_t));

    if ((flags & NORMAL_OBSTACLE_FRIEND) == NORMAL_OBSTACLE_FRIEND) {
        float min_dist_friend = FLT_MAX;
        for (uint8_t i = 1; i < 4; i++) {
            float dist_to_friend = RobotDist2Point(shared_robot_data[i]->pose.x, shared_robot_data[i]->pose.y);
            float angle_to_friend = RobotAngletoPoint(shared_robot_data[i]->pose.x, shared_robot_data[i]->pose.y);

            // printf("Hello: %d %.2f %.2f || %.2f %.2f || %.2f %.2f\n", i, shared_robot_data[i]->pose.x, shared_robot_data[i]->pose.y, dist_to_friend, deltaAngle(angle_to_friend, theta), angle_to_friend, theta);

            // Skip ketika melihat obstacle lebih dekat
            if ((flags & NORMAL_OBSTACLE) == NORMAL_OBSTACLE || (flags & NORMAL_OBSTACLE_BALL) == NORMAL_OBSTACLE_BALL) {
                if (ret_temp_temp.status == 1 && ret_temp_temp.distance < dist_to_friend) {
                    continue;
                }
            }

            if (dist_to_friend < min_dist_friend && dist_to_friend < dist && deltaAngle(angle_to_friend, theta) < tolerance) {
                min_dist_friend = dist_to_friend;
                ret_temp.pos_x = shared_robot_data[i]->pose.x;
                ret_temp.pos_y = shared_robot_data[i]->pose.y;
                ret_temp.distance = dist_to_friend;
                ret_temp.angle = angle_to_friend;

                last_obs_x = ret_temp.pos_x;
                last_obs_y = ret_temp.pos_y;

                // printf("RET9: %.2f %.2f\n", last_obs_x, last_obs_y);

                ret_temp.status = 1;
            }
        }
    }

    last_obs_x = ret_temp.pos_x;
    last_obs_y = ret_temp.pos_y;

    // printf("RET10: %.2f %.2f\n", last_obs_x, last_obs_y);

    return ret_temp;
}

target_goal_t Motion::sudut_robot_ke_gawang_lawan()
{
    target_goal_t ret_buffer;
    if (*empty_goal_status == 1) {
        ret_buffer.goal_tengah[0] = *target_goal[0];
        ret_buffer.goal_tengah[1] = *target_goal[1];
        ret_buffer.goal_kiri[0] = *min_tolerance_empty_goal_on_field[0];
        ret_buffer.goal_kiri[1] = *min_tolerance_empty_goal_on_field[1];
        ret_buffer.goal_kanan[0] = *max_tolerance_empty_goal_on_field[0];
        ret_buffer.goal_kanan[1] = *max_tolerance_empty_goal_on_field[1];
        ret_buffer.status_gawang_kosong = 1;
        // LogWithColor("magenta", "EMPTY GOAL target %d %.2f %.2f || kiri %.2f %.2f || kanan %.2f %.2f\n",
        //              ret_buffer.goal_tengah[0], ret_buffer.goal_tengah[1],
        //              ret_buffer.goal_kiri[0], ret_buffer.goal_kiri[1],
        //              ret_buffer.goal_kanan[0], ret_buffer.goal_kanan[1]);
    } else {
        if (final_pose->x < FIELD_X_1_2) {
            ret_buffer.goal_tengah[0] = 460;
            ret_buffer.goal_tengah[1] = 1200;
            ret_buffer.status_gawang_kosong = 0;
            // LogWithColor("green", "EMPTY GOAL %d %.2f %.2f\n",
            //              *empty_goal_status, ret_buffer.goal_tengah[0], ret_buffer.goal_tengah[1]);
        } else {
            ret_buffer.goal_tengah[0] = 340;
            ret_buffer.goal_tengah[1] = 1200;
            ret_buffer.status_gawang_kosong = 0;
            // LogWithColor("green", "EMPTY GOAL %d %.2f %.2f\n",
            //              *empty_goal_status, ret_buffer.goal_tengah[0], ret_buffer.goal_tengah[1]);
        }
    }

    return ret_buffer;
}
uint8_t Motion::motionAngleControlPivot(float vx_local, float vy_local, float target_theta, float vtheta)
{
    static PID angular_pid(1.8, 0, 0); // 3

    float error_th = target_theta - final_pose->theta;

    while (error_th > 180)
        error_th -= 360;
    while (error_th < -180)
        error_th += 360;

    float output_theta = angular_pid.calculate(error_th, vtheta);

    float output_x = output_theta * M_PI / 180 * 30;
    float output_y = fabs(output_x * 3);

    if (output_y > 100)
        output_y = 100;
    else if (output_y < -100)
        output_y = -100;

    manualMotion(output_x + vx_local, vy_local, output_theta);

    printf("error: %.2f %.2f %.2f || %.2f %.2f\n", target_theta, final_pose->theta, error_th, output_x, output_y);

    if (fabs(error_th) < 1.5)
        return 1;
    else
        return 0;
}

uint8_t Motion::hadapGawangVision(float vx_local, float vy_local, float vtheta)
{
    static PID angular_pid(2.5, 0, 0);

    float error_angular = 90 - *target_goal_vision;
    if (error_angular > 180)
        error_angular -= 360;
    else if (error_angular < -180)
        error_angular += 360;

    float output_angular = angular_pid.calculate(error_angular, vtheta) * -1;

    manualMotion(vx_local, vy_local, output_angular);

    if (fabs(error_angular) < 2) {
        return 1;
    } else {
        return 0;
    }
}