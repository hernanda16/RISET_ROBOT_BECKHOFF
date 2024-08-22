#include "world_model/ball.hpp"

double regress(double x)
{
    double terms[] = {
        4.7065381245900983e-001,
        -1.4081289132500154e-002,
        4.0924833493510886e-004
    };

    size_t csz = sizeof terms / sizeof *terms;

    double t = 1;
    double r = 0;
    for (int i = 0; i < csz; i++) {
        r += terms[i] * t;
        t *= x;
    }
    return r;
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "ball_node");
    ros::NodeHandle nh;
    ros::MultiThreadedSpinner spinner(0);

    pub_ball_final_pos = nh.advertise<geometry_msgs::Pose2D>("/world_model/ball/final_pos", 1);
    pub_ball_final_vel = nh.advertise<geometry_msgs::Pose2D>("/world_model/ball/final_vel", 1);
    pub_ball_status = nh.advertise<std_msgs::UInt8>("/world_model/ball/status", 1);
    pub_ball_waypoints = nh.advertise<sensor_msgs::PointCloud>("/world_model/ball/waypoints", 1);
    sub_vision_ball_data = nh.subscribe("/vision/data/ball", 1, callbackSubcribeVisionDataBall);
    sub_robot_pose = nh.subscribe<geometry_msgs::Pose2D>("/world_model/robot/pose", 1, callbackSubcribeRobotPose);
    sub_obs_on_field = nh.subscribe<sensor_msgs::PointCloud>("/world_model/obs/pos", 1, callbackSubcribeObsOnField);
    sub_shared_data = nh.subscribe<iris_msgs::data_robots>("/communication/icast/data_robot", 1, callbackSubcribeSharedData);

    tim_routine = nh.createTimer(ros::Duration(0.02), callbackRoutine);
    loadLUT(LUTfr2lap);

    memset(&robot_pose, 0, sizeof(pose2d_t));

    init_kf_vars();

    A << 1, dt, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, dt,
        0, 0, 0, 1;
    C << 1, 0, 0, 0,
        0, 0, 1, 0;
    Q << 0.55, 0, 0, 0,
        0, 0.55, 0, 0,
        0, 0, 0.55, 0,
        0, 0, 0, 0.55;
    R << 81, 0,
        0, 81;
    P << 1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1;

    // A << 1, dt, 0.5 * dt * dt, 0, 0, 0,
    //     0, 1, dt, 0, 0, 0,
    //     0, 0, 1, 0, 0, 0,
    //     0, 0, 0, 1, dt, 0.5 * dt * dt,
    //     0, 0, 0, 0, 1, dt,
    //     0, 0, 0, 0, 0, 1;
    // C << 1, 0, 0, 0, 0, 0,
    //     0, 0, 0, 1, 0, 0;
    // Q << 0.05, 0, 0, 0, 0, 0,
    //     0, 0.05, 0, 0, 0, 0,
    //     0, 0, 0.05, 0, 0, 0,
    //     0, 0, 0, 0.05, 0, 0,
    //     0, 0, 0, 0, 0.05, 0,
    //     0, 0, 0, 0, 0, 0.05;
    // R << 49, 0,
    //     0, 49;
    // P << 1, 0, 0, 0, 0, 0,
    //     0, 1, 0, 0, 0, 0,
    //     0, 0, 1, 0, 0, 0,
    //     0, 0, 0, 1, 0, 0,
    //     0, 0, 0, 0, 1, 0,
    //     0, 0, 0, 0, 0, 1;

    kf_ball = KalmanFilter(dt, A, C, Q, R, P);

    Eigen::VectorXd x0(n);
    t = 0;
    x0 << 0, 0, 0, 0;
    kf_ball.init(t, x0);

    spinner.spin();
    return 0;
}

void callbackRoutine(const ros::TimerEvent& event)
{
    static differential_v_const_t temp_diff_final_ball_x, temp_diff_final_ball_y;
    static point2d_t prev_temp_final_ball = { 0, 0 };

    mtx_ball.lock();
    point2d_t temp_final_ball = fuseBalls_2023();
    mtx_ball.unlock();

    std_msgs::UInt8 ball_status_msg;
    ball_status_msg.data = ball_status;
    pub_ball_status.publish(ball_status_msg);

    if (ball_status > 0 && ball_status != prev_ball_status) {
        Eigen::VectorXd x0(n);
        t = 0;
        x0 << temp_final_ball.x, 0, temp_final_ball.y, 0;
        kf_ball.init(t, x0);
    }
    prev_ball_status = ball_status;

    if (ball_status_msg.data == 0) {
        return;
    }

    prev_temp_final_ball.x = temp_final_ball.x;
    prev_temp_final_ball.y = temp_final_ball.y;

    //================================================================================================
    static const float complemntary_gain = 1;
    float dx = temp_final_ball.x - robot_pose.x;
    float dy = temp_final_ball.y - robot_pose.y;
    float arah_bola_ke_robot = atan2(dy, dx) * 180 / M_PI - 180;
    float kecepatan_magnitude_robot = pythagoras(robot_vel.x, robot_vel.y, 0, 0);
    float kecepatan_complement = regress(kecepatan_magnitude_robot);

    float ball_complement_x = temp_final_ball.x + cosf(arah_bola_ke_robot * M_PI / 180) * kecepatan_complement;
    float ball_complement_y = temp_final_ball.y + sinf(arah_bola_ke_robot * M_PI / 180) * kecepatan_complement;

    temp_final_ball.x = temp_final_ball.x * (1 - complemntary_gain) + ball_complement_x * complemntary_gain;
    temp_final_ball.y = temp_final_ball.y * (1 - complemntary_gain) + ball_complement_y * complemntary_gain;
    //================================================================================================

    Eigen::VectorXd y(m);
    t += dt;
    y << temp_final_ball.x, temp_final_ball.y;
    kf_ball.update(y);

    temp_diff_final_ball_x.x = kf_ball.state().transpose()[0];
    temp_diff_final_ball_x.x_dot = kf_ball.state().transpose()[1];
    temp_diff_final_ball_y.x = kf_ball.state().transpose()[2];
    temp_diff_final_ball_y.x_dot = kf_ball.state().transpose()[3];

    if (ball_status != BALL_STATUS_OPPONENT) {
        checkBallOnOpponent(point2d_t { temp_diff_final_ball_x.x, temp_diff_final_ball_y.x });
    }

    std::vector<point2d_t> ball_waypoints = createBallWaypoints(temp_diff_final_ball_x, temp_diff_final_ball_y);

    //================================================================================================
    // float dx = temp_diff_final_ball_x.x - robot_pose.x;
    // float dy = temp_diff_final_ball_y.x - robot_pose.y;
    // float jarak_robot_ke_bola = pythagoras(temp_diff_final_ball_x.x, temp_diff_final_ball_y.x, robot_pose.x, robot_pose.y);
    // float arah_robot_ke_bola = atan2(dy, dx) * 180 / M_PI;
    // float kecepatan_robot = pythagoras(robot_vel.x, robot_vel.y, 0, 0);

    // float error = pythagoras(temp_diff_final_ball_x.x_dot, temp_diff_final_ball_y.x_dot, 0, 0);
    // float error_angle = atan2(temp_diff_final_ball_y.x_dot, temp_diff_final_ball_x.x_dot) * 180 / M_PI;

    // printf("%.2f %.2f %.2f %.2f\n", kecepatan_robot, arah_robot_ke_bola, error, error_angle);

    //================================================================================================

    // TITIP
    // printf("%.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n",
    //        temp_diff_final_ball_x.x, temp_diff_final_ball_y.x,
    //        temp_final_ball.x, temp_final_ball.y,
    //        ball_distance_frame, ball_angle_frame,
    //        pythagoras(temp_diff_final_ball_x.x, temp_diff_final_ball_y.x, robot_pose.x, robot_pose.y),
    //        pythagoras(temp_final_ball.x, temp_final_ball.y, robot_pose.x, robot_pose.y), ball_frame_x, ball_frame_y);

    geometry_msgs::Pose2D ball_final_pose_msg;
    geometry_msgs::Pose2D ball_final_vel_msg;

    ball_final_pose_msg.x = temp_diff_final_ball_x.x;
    ball_final_pose_msg.y = temp_diff_final_ball_y.x;

    while (bola_theta > 180)
        bola_theta -= 360;
    while (bola_theta < -180)
        bola_theta += 360;

    ball_final_pose_msg.theta = bola_theta;
    ball_final_vel_msg.x = temp_diff_final_ball_x.x_dot;
    ball_final_vel_msg.y = temp_diff_final_ball_y.x_dot;

    sensor_msgs::PointCloud ball_waypoints_msg;
    size_t ball_waypoints_size = ball_waypoints.size();
    for (size_t i = 0; i < ball_waypoints_size; i++) {
        geometry_msgs::Point32 temp_point;
        temp_point.x = ball_waypoints[i].x;
        temp_point.y = ball_waypoints[i].y;
        ball_waypoints_msg.points.push_back(temp_point);
    }

    pub_ball_final_pos.publish(ball_final_pose_msg);
    pub_ball_final_vel.publish(ball_final_vel_msg);
    pub_ball_waypoints.publish(ball_waypoints_msg);

    prev_ball_status = ball_status;
}

void callbackSubcribeVisionDataBall(const sensor_msgs::PointCloudConstPtr& msg)
{
    mtx_ball.lock();
    ball.clear();
    my_ball_in_world.clear();
    size_t size = msg->points.size();
    for (uint8_t i = 0; i < size; i++) {
        point2d_t temp;
        temp.x = msg->points[i].x;
        temp.y = msg->points[i].y;
        ball.push_back(temp);

        // TITIP
        ball_frame_x = temp.x;
        ball_frame_y = temp.y;
        ball_distance_frame = pythagoras(ball_frame_x, ball_frame_y, 0, 0);
        ball_angle_frame = atan2(ball_frame_y, ball_frame_x) * 180 / M_PI;

        balls_vote_t temp_ball_world;
        polar2d_t ball_polar;
        frameToWorld(temp, &temp_ball_world.position, &ball_polar, &robot_pose, LUTfr2lap);

        // printf("ball: %.2f %.2f\n", ball_polar.r, ball_polar.theta);

        // if (isInsideField(temp_ball_world.position))
        // {
        my_ball_in_world.push_back(temp_ball_world);

        bola_theta = ball_polar.theta;
        // }
    }
    mtx_ball.unlock();
}

void callbackSubcribeRobotPose(const geometry_msgs::Pose2DConstPtr& msg)
{
    static double last_time = ros::Time::now().toSec();

    double time_now = ros::Time::now().toSec();

    prev_robot_pose.x = robot_pose.x;
    prev_robot_pose.y = robot_pose.y;
    prev_robot_pose.theta = robot_pose.theta;

    robot_pose.x = msg->x;
    robot_pose.y = msg->y;
    robot_pose.theta = msg->theta;

    robot_vel.x = (robot_pose.x - prev_robot_pose.x) / (time_now - last_time);
    robot_vel.y = (robot_pose.y - prev_robot_pose.y) / (time_now - last_time);
    robot_vel.theta = (robot_pose.theta - prev_robot_pose.theta) / (time_now - last_time);

    last_time = time_now;
}

void callbackSubcribeObsOnField(const sensor_msgs::PointCloudConstPtr& msg)
{
    obs_on_field.clear();
    size_t size = msg->points.size();
    for (uint8_t i = 0; i < size; i++) {
        point2d_t temp;
        temp.x = msg->points[i].x;
        temp.y = msg->points[i].y;
        obs_on_field.push_back(temp);
    }
}

point2d_t fuseBalls_2023()
{
    point2d_t ret_temp;
    memset(&ret_temp, 0, sizeof(point2d_t));

    int8_t ball_vision = (my_ball_in_world.size() > 0);

    if (ball_vision == 1) {
        ret_temp.x = my_ball_in_world[0].position.x;
        ret_temp.y = my_ball_in_world[0].position.y;

        ball_status = BALL_STATUS_SELF;
    } else {
        float min_dist = FLT_MAX;
        point2d_t friend_ball;
        uint8_t friend_ball_status = 0;

        for (size_t i = 1; i < 4; i++) {
            if (shared_robot_data[i].ball_status == 0 || shared_robot_data[i].ball_status == 3)
                continue;
            float jarak_robot_ke_bola = pythagoras(shared_robot_data[i].pose.x, shared_robot_data[i].pose.y, shared_robot_data[i].ball.x, shared_robot_data[i].ball.y);

            if (jarak_robot_ke_bola < min_dist) {
                jarak_robot_ke_bola = min_dist;
                friend_ball.x = shared_robot_data[i].ball.x;
                friend_ball.y = shared_robot_data[i].ball.y;

                friend_ball_status = 1;
            }
        }

        if (friend_ball_status == 1) {
            ret_temp.x = friend_ball.x;
            ret_temp.y = friend_ball.y;

            ball_status = BALL_STATUS_FRIEND;
        } else {
            ball_status = BALL_STATUS_NOT_SEEN;
        }
    }

    return ret_temp;
}

point2d_t fuseBalls()
{
    /**
     * Use knn-like algorithm to vote,
     * just simple counter vote
     */

    point2d_t ret_temp;
    memset(&ret_temp, 0, sizeof(point2d_t));
    std::vector<balls_vote_t> balls_in_world;

    ball_status = BALL_STATUS_NOT_SEEN;

    size_t my_ball_in_world_size = my_ball_in_world.size();
    size_t friends_ball_in_world_size = friends_ball_in_world.size();

    if (my_ball_in_world_size == 0 && friends_ball_in_world_size == 0)
        return ret_temp;

    ball_status = BALL_STATUS_SELF;

    // printf("ADA  SEBELUM : %ld\n", balls_in_world.size());

    balls_in_world.insert(balls_in_world.end(), my_ball_in_world.begin(), my_ball_in_world.end());
    balls_in_world.insert(balls_in_world.end(), friends_ball_in_world.begin(), friends_ball_in_world.end());

    // printf("ADA  SESUDAH : %ld\n", balls_in_world.size());
    // printf("ADA BERAPA? %ld\n", friends_ball_in_world.size());

    size_t balls_in_world_size = balls_in_world.size();

    for (size_t i = 0; i < balls_in_world_size; i++) {
        balls_in_world[i].vote_count = 0;
    }

    for (size_t i = 0; i < balls_in_world_size; i++) {
        for (size_t j = i + 1; j < balls_in_world_size; j++) // Incremental search
        {
            float distance = pythagoras(balls_in_world[i].position, balls_in_world[j].position);
            if (distance < nearest_ball_threshold)
                balls_in_world[i].vote_count++;
        }
    }

    uint16_t max_vote = balls_in_world[0].vote_count;
    ret_temp.x = balls_in_world[0].position.x;
    ret_temp.y = balls_in_world[0].position.y;
    for (size_t i = 0; i < balls_in_world_size; i++) {
        printf("ADA ------ : %.2f\n", balls_in_world[i].position.x);

        printf(" ========== %d %d\n", balls_in_world[i].vote_count, max_vote);

        if (balls_in_world[i].vote_count >= max_vote) {
            ret_temp.x = balls_in_world[i].position.x;
            ret_temp.y = balls_in_world[i].position.y;

            max_vote = balls_in_world[i].vote_count;

            // if (i >= my_ball_in_world_size)
            ball_status = BALL_STATUS_FRIEND;
        }
    }

    return ret_temp;
}

std::vector<point2d_t> createBallWaypoints(differential_v_const_t ball_x, differential_v_const_t ball_y, float time_max_s, float time_step_s)
{
    std::vector<point2d_t> ret_temp;

    point2d_t pos_ball;
    point2d_t vel_ball;

    pos_ball.x = ball_x.x;
    pos_ball.y = ball_y.x;

    vel_ball.x = ball_x.x_dot;
    vel_ball.y = ball_y.x_dot;

    ret_temp.push_back(pos_ball);

    float vel_ball_mag = pythagoras(vel_ball, point2d_t { 0, 0 });

    // 50cm/s
    if (vel_ball_mag < 4) {
        return ret_temp;
    }

    for (float t = time_step_s; t < time_max_s; t += time_step_s) {
        point2d_t temp;
        temp.x = pos_ball.x + vel_ball.x * t;
        temp.y = pos_ball.y + vel_ball.y * t;

        ret_temp.push_back(temp);
    }

    // point2d_t temp;
    // temp.x = pos_ball.x + vel_ball.x * 0.4;
    // temp.y = pos_ball.y + vel_ball.y * 0.4;
    // ret_temp.push_back(temp);
    // temp.x = pos_ball.x + vel_ball.x * 1;
    // temp.y = pos_ball.y + vel_ball.y * 1;
    // ret_temp.push_back(temp);

    return ret_temp;
}

void init_kf_vars()
{
    kf_final_ball_x.has_init = 0;

    kf_final_ball_x.uncertainly_est.x = 4;
    kf_final_ball_x.uncertainly_est.x_dot = 1;
    kf_final_ball_x.uncertainly_mea.x = 16;
    kf_final_ball_x.uncertainly_mea.x_dot = 81;
    kf_final_ball_x.q = 0.5;

    //--------------------------------------------------------------------------------

    kf_final_ball_y.has_init = 0;

    kf_final_ball_y.uncertainly_est.x = 4;
    kf_final_ball_y.uncertainly_est.x_dot = 1;
    kf_final_ball_y.uncertainly_mea.x = 16;
    kf_final_ball_y.uncertainly_mea.x_dot = 81;
    kf_final_ball_y.q = 0.5;
}

void checkBallOnOpponent(point2d_t ball_on_field)
{

    size_t obs_on_field_size = obs_on_field.size();
    for (size_t i = 0; i < obs_on_field_size; i++) {
        float distance = pythagoras(obs_on_field[i], ball_on_field);
        if (distance < opponent_ball_distance_threshold) {
            ball_status = BALL_STATUS_OPPONENT;
            first_time_opponent_has_ball = ros::Time::now().toSec();
            return;
        }
    }
}

void callbackSubcribeSharedData(const iris_msgs::data_robotsConstPtr& msg)
{
    int8_t robot_num = atoi(getenv("ROBOT"));

    friends_ball_in_world.clear();

    for (uint8_t i = 1; i < msg->pose_x.size(); i++) {
        shared_robot_data[i].pose.x = msg->pose_x.at(i);
    }
    for (uint8_t i = 1; i < msg->pose_y.size(); i++) {
        shared_robot_data[i].pose.y = msg->pose_y.at(i);
    }

    // if (robot_num != 1) {
    //     for (int8_t i = 0; i < msg->ball_pos_x_1.size(); i++) {
    //         balls_vote_t temp_ball;
    //         temp_ball.position.x = msg->ball_pos_x_1[i];
    //         temp_ball.position.y = msg->ball_pos_y_1[i];

    //         if (temp_ball.position.x > __FLT_EPSILON__ || temp_ball.position.y > __FLT_EPSILON__ || temp_ball.position.x < -__FLT_EPSILON__ || temp_ball.position.y < -__FLT_EPSILON__)
    //             friends_ball_in_world.push_back(temp_ball);
    //     }

    //     if (msg->ball_pos_x_1.size() > 0) {
    //         shared_robot_data[1].ball.x = msg->ball_pos_x_1[0];
    //         shared_robot_data[1].ball.y = msg->ball_pos_y_1[0];
    //     }

    //     shared_robot_data[1].ball_status = msg->ball_is_visible.at(1);
    // }

    if (robot_num != 2) {
        for (int8_t i = 0; i < msg->ball_pos_x_2.size(); i++) {
            balls_vote_t temp_ball;
            temp_ball.position.x = msg->ball_pos_x_2[i];
            temp_ball.position.y = msg->ball_pos_y_2[i];

            if (temp_ball.position.x > __FLT_EPSILON__ || temp_ball.position.y > __FLT_EPSILON__ || temp_ball.position.x < -__FLT_EPSILON__ || temp_ball.position.y < -__FLT_EPSILON__)
                friends_ball_in_world.push_back(temp_ball);
        }
        if (msg->ball_pos_x_2.size() > 0) {
            shared_robot_data[2].ball.x = msg->ball_pos_x_2[0];
            shared_robot_data[2].ball.y = msg->ball_pos_y_2[0];
        }
        shared_robot_data[2].ball_status = msg->ball_is_visible.at(2);
    }

    if (robot_num != 3) {
        for (int8_t i = 0; i < msg->ball_pos_x_3.size(); i++) {
            balls_vote_t temp_ball;
            temp_ball.position.x = msg->ball_pos_x_3[i];
            temp_ball.position.y = msg->ball_pos_y_3[i];

            if (temp_ball.position.x > __FLT_EPSILON__ || temp_ball.position.y > __FLT_EPSILON__ || temp_ball.position.x < -__FLT_EPSILON__ || temp_ball.position.y < -__FLT_EPSILON__)
                friends_ball_in_world.push_back(temp_ball);
        }

        if (msg->ball_pos_x_3.size() > 0) {
            shared_robot_data[3].ball.x = msg->ball_pos_x_3[0];
            shared_robot_data[3].ball.y = msg->ball_pos_y_3[0];
        }
        shared_robot_data[3].ball_status = msg->ball_is_visible.at(3);
    }

    // printf("ADA BERAPA? %ld\n", friends_ball_in_world.size());
}
