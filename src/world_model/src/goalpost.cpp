#include "world_model/goalpost.hpp"

int main(int argc, char** argv)
{
    ros::init(argc, argv, "goalpost_node");
    ros::NodeHandle nh;
    ros::MultiThreadedSpinner spinner(0);
    image_transport::ImageTransport IT(nh);

    // Load configuration
    loadConfig();
    loadLUT(LUTlap2fr, false);
    loadLUT(LUTfr2lap, true);

    sub_robot_pose = nh.subscribe<geometry_msgs::Pose2D>("/world_model/robot/pose", 1, callbackSubcribeRobotPose);
    sub_frame_bgr = IT.subscribe("/vision/frame_bgr", 1, callbackSubcribeFrameBGR);
    sub_control_box = nh.subscribe("/threshold_params", 1, callbackSubControlBox);

    pub_goalpost = nh.advertise<iris_msgs::goalpost>("/world_model/goalpost/data", 1);
    pub_frame_goalpost = IT.advertise("/world_model/display/goalpost", 1);
    pub_frame_goalpost_binary = IT.advertise("world_model/debug/goalpost", 1);

    tim_routine = nh.createTimer(ros::Duration(0.1), callbackRoutine);

    spinner.spin();

    return 0;
}

void callbackSubcribeFrameBGR(const sensor_msgs::ImageConstPtr& msg)
{
    mutex_frame_bgr.lock();
    try {
        frame_bgr = cv_bridge::toCvShare(msg, "bgr8")->image.clone();
        SET_BIT(flag, 0);
    } catch (cv_bridge::Exception& e) {
        ROS_ERROR("cv_bridge exception: %s", e.what());
    }
    mutex_frame_bgr.unlock();
}

void callbackSubControlBox(const std_msgs::UInt16MultiArray::ConstPtr& msg)
{
    /**
     * This is callback from web_adapter
     * Index 0 is data identifier
     * Index 1 is data lenght
     * Index 2 and more is the data
     *
     * For the datas, see controlbox_data variable
     */
    memcpy(controlbox_data + msg->data[0] * 6, &msg->data[0] + 2, msg->data[1] * sizeof(uint16_t));
}

void loadConfig()
{
    char* robot_num;
    robot_num = getenv("ROBOT");
    char config_file[100], config_file_2[100];
    std::string current_dir = ros::package::getPath("world_model");
    sprintf(config_file, "%s/../../config/IRIS%s/static_conf.yaml", current_dir.c_str(), robot_num);
    sprintf(config_file_2, "%s/../../config/IRIS%s/dynamic_conf.yaml", current_dir.c_str(), robot_num);

    YAML::Node config = YAML::LoadFile(config_file);
    center_cam_x = config["Camera"]["cam_offset_x"].as<int>();
    center_cam_y = config["Camera"]["cam_offset_y"].as<int>();

    center_cam_x += (int)(g_res_x * 0.5);
    center_cam_y += (int)(g_res_y * 0.5);

    YAML::Node config_dynamic = YAML::LoadFile(config_file_2);
    controlbox_data[18] = config_dynamic["Vision"]["keeper_obs_y_min"].as<int>();
    controlbox_data[19] = config_dynamic["Vision"]["keeper_obs_y_max"].as<int>();
    controlbox_data[20] = config_dynamic["Vision"]["keeper_obs_u_min"].as<int>();
    controlbox_data[21] = config_dynamic["Vision"]["keeper_obs_u_max"].as<int>();
    controlbox_data[22] = config_dynamic["Vision"]["keeper_obs_v_min"].as<int>();
    controlbox_data[23] = config_dynamic["Vision"]["keeper_obs_v_max"].as<int>();
}

void callbackRoutine(const ros::TimerEvent& event)
{
    if (flag == 0) {
        return;
    }

    mutex_frame_bgr.lock();
    Mat frame_bgr_copy = frame_bgr.clone();
    CLEAR_BIT(flag, 0);
    mutex_frame_bgr.unlock();

    static uint8_t is_goal_empty;

    static Point_frame_field enemy_goal;
    static Point_frame_field empty_goal;
    static Point_frame_field gawang_kiri;
    static Point_frame_field gawang_kanan;

    static Point_frame_field empty_goal_min_tolerance;
    static Point_frame_field empty_goal_max_tolerance;

    static Mat null_frame(g_res_y, g_res_x, CV_8UC1, Scalar(0));
    static Mat frame_goal_keeper_bgr = Mat::zeros(g_res_y, g_res_x, CV_8UC3);
    static Mat frame_yuv_buffer;

    static const float keeper_pos_on_field[2] = { 400, 1250 };
    static float real_keeper_on_field[2] = { 300, 0 };

    //---KEEPER ENEMY THRESHOLD
    //=========
    enemy_goal.field_x = keeper_pos_on_field[0];
    enemy_goal.field_y = keeper_pos_on_field[1];

    gawang_kiri.field_x = 330;
    gawang_kiri.field_y = 1250;
    gawang_kanan.field_x = 470;
    gawang_kanan.field_y = 1250;

    frameFieldCaluculate(&enemy_goal);
    null_frame = Mat::zeros(g_res_y, g_res_x, CV_8UC1);
    frame_goal_keeper_bgr = frame_bgr_copy.clone();
    cv::Mat anjay_frame = frame_bgr_copy.clone();
    cv::Mat raw_empty_goal = frame_bgr_copy.clone();
    cv::Mat frame_binary_empty_goal;
    cv::Mat frame_goalkeeper_hsv;
    // cvtColor(frame_bgr_copy, anjay_frame, CV_YUV2BGR);
    // cvtColor(frame_bgr_copy, frame_goal_keeper_bgr, CV_YUV2BGR);
    // cvtColor(frame_bgr_copy, raw_empty_goal, CV_YUV2BGR);
    cvtColor(raw_empty_goal, frame_goalkeeper_hsv, CV_BGR2HSV);

    // {
    //     frameFieldCaluculate(&gawang_kiri);
    //     frameFieldCaluculate(&gawang_kanan);

    //     float origin_x = center_cam_x;
    //     float origin_y = center_cam_y;
    //     float gawang_x_pada_frame = enemy_goal.frame_x;
    //     float gawang_y_pada_frame = enemy_goal.frame_y;

    //     float dx = gawang_x_pada_frame - origin_x;
    //     float dy = gawang_y_pada_frame - origin_y;

    //     float r = sqrtf(dx * dx + dy * dy);
    //     float R = r + 7;
    //     float a = atan2f(dy, dx);

    //     float x_R = origin_x + R * cosf(a);
    //     float y_R = origin_y + R * sinf(a);

    //     // -----
    //     // RotatedRect rRect = RotatedRect(Point2f(x_R, y_R), Size2f(20, 100), a RAD2DEG);
    //     RotatedRect rRect = RotatedRect(Point2f(x_R, y_R), Size2f(20, 80), a RAD2DEG);

    //     Point2f vertices[4];
    //     rRect.points(vertices);

    //     std::vector<std::vector<cv::Point>> sss;
    //     std::vector<cv::Point> ss;
    //     ss.push_back(cv::Point(vertices[0].x, vertices[0].y));
    //     ss.push_back(cv::Point(vertices[1].x, vertices[1].y));
    //     ss.push_back(cv::Point(vertices[2].x, vertices[2].y));
    //     ss.push_back(cv::Point(vertices[3].x, vertices[3].y));
    //     sss.push_back(ss);
    //     fillPoly(null_frame, sss, Scalar(255));
    //     // -----
    // }

    {
        frameFieldCaluculate(&gawang_kiri);
        frameFieldCaluculate(&gawang_kanan);

        float sudut_robot_ke_gawang_field = atan2f(1200 - robot_pose.y, 400 - robot_pose.x) - M_PI_2;

        // printf("asd: %.2f\n", sudut_robot_ke_gawang_field);

        float origin_x = center_cam_x;
        float origin_y = center_cam_y;
        float gawang_x_pada_frame = enemy_goal.frame_x;
        float gawang_y_pada_frame = enemy_goal.frame_y;

        float dx = gawang_x_pada_frame - origin_x;
        float dy = gawang_y_pada_frame - origin_y;

        float r = sqrtf(dx * dx + dy * dy);
        float R = r + 12;
        float a = atan2f(dy, dx);

        float x_R = origin_x + R * cosf(a);
        float y_R = origin_y + R * sinf(a);
        //

        float dx_kiri = gawang_kiri.frame_x - origin_x;
        float dy_kiri = gawang_kiri.frame_y - origin_y;
        float r_kiri = sqrtf(dx_kiri * dx_kiri + dy_kiri * dy_kiri);
        float R_kiri = r_kiri + 12;
        float a_kiri = atan2f(dy_kiri, dx_kiri);

        float x_R_kiri = origin_x + R_kiri * cosf(a_kiri);
        float y_R_kiri = origin_y + R_kiri * sinf(a_kiri);

        float dx_kanan = gawang_kanan.frame_x - origin_x;
        float dy_kanan = gawang_kanan.frame_y - origin_y;
        float r_kanan = sqrtf(dx_kanan * dx_kanan + dy_kanan * dy_kanan);
        float R_kanan = r_kanan + 12;
        float a_kanan = atan2f(dy_kanan, dx_kanan);

        float x_R_kanan = origin_x + R_kanan * cosf(a_kanan);
        float y_R_kanan = origin_y + R_kanan * sinf(a_kanan);

        std::vector<std::vector<cv::Point>> sss;
        std::vector<cv::Point> ss;
        ss.push_back(cv::Point(gawang_kiri.frame_x, gawang_kiri.frame_y));
        ss.push_back(cv::Point(gawang_x_pada_frame, gawang_y_pada_frame));
        ss.push_back(cv::Point(gawang_kanan.frame_x, gawang_kanan.frame_y));
        ss.push_back(cv::Point(x_R_kanan, y_R_kanan));
        ss.push_back(cv::Point(x_R, y_R));
        ss.push_back(cv::Point(x_R_kiri, y_R_kiri));

        sss.push_back(ss);
        fillPoly(null_frame, sss, Scalar(255));
        // -----
    }

    cv::Mat masked_bgr;
    bitwise_and(frame_goalkeeper_hsv, frame_goalkeeper_hsv, masked_bgr, null_frame);

    //==================================================================

    if (controlbox_data[18] > controlbox_data[19]) {
        Mat dstA, dstB;
        inRange(masked_bgr, Scalar(controlbox_data[18], controlbox_data[20], controlbox_data[22]), Scalar(controlbox_data[19], controlbox_data[21], controlbox_data[23]), dstA);
        inRange(masked_bgr, Scalar(controlbox_data[18], controlbox_data[20], controlbox_data[22]), Scalar(255, controlbox_data[21], controlbox_data[23]), dstB);
        bitwise_or(dstA, dstB, frame_binary_empty_goal);
    } else {
        inRange(masked_bgr, Scalar(controlbox_data[18], controlbox_data[20], controlbox_data[22]), Scalar(controlbox_data[19], controlbox_data[21], controlbox_data[23]), frame_binary_empty_goal);
    }

    float sudut_robot_ke_gawang_vision;

    vector<vector<Point>> empty_goal_contour;
    vector<Vec4i> empty_goal_hierarki;
    findContours(frame_binary_empty_goal, empty_goal_contour, empty_goal_hierarki, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    float largest_empty_goal = 0;
    uint16_t largest_empty_goal_index = 0;

    uint8_t lock_goal_kiri_atau_kanan = 0;

    int frame_x_target_2024 = enemy_goal.frame_x;
    int frame_y_target_2024 = enemy_goal.frame_y;

    if (empty_goal_contour.size()) {
        for (uint16_t i = 0; i < empty_goal_contour.size(); i++) {
            if (contourArea(empty_goal_contour[i], false) > largest_empty_goal) {
                largest_empty_goal = contourArea(empty_goal_contour[i], false);
                largest_empty_goal_index = i;
            }
        }
        Rect rect = boundingRect(empty_goal_contour[largest_empty_goal_index]);

        frame_x_target_2024 = rect.x + rect.width * 0.5;
        frame_y_target_2024 = rect.y + rect.height * 0.5;

        int x_kiri, x_kanan;
        int y_kiri, y_kanan;
        x_kiri = rect.x + 0;
        x_kanan = rect.x + rect.width;
        for (int i = 0; i < empty_goal_contour[largest_empty_goal_index].size(); i++) {
            if (empty_goal_contour[largest_empty_goal_index][i].x == rect.x + 0)
                y_kiri = empty_goal_contour[largest_empty_goal_index][i].y;
            if (empty_goal_contour[largest_empty_goal_index][i].x == rect.x + rect.width - 1)
                y_kanan = empty_goal_contour[largest_empty_goal_index][i].y;
        }

        float origin_x = center_cam_x;
        float origin_y = center_cam_y;
        float gawang_x_pada_frame = enemy_goal.frame_x;
        float gawang_y_pada_frame = enemy_goal.frame_y;

        float dx = origin_x - gawang_x_pada_frame;
        float dy = origin_y - gawang_y_pada_frame;

        float a = atan2f(dy, dx);

        int x_kiri_offset = x_kiri + 20 * cosf(a);
        int y_kiri_offset = y_kiri + 20 * sinf(a);
        int x_kanan_offset = x_kanan + 20 * cosf(a);
        int y_kanan_offset = y_kanan + 20 * sinf(a);

        empty_goal_min_tolerance.frame_x = x_kiri_offset - center_cam_x;
        empty_goal_min_tolerance.frame_y = center_cam_y - y_kiri_offset;
        empty_goal_min_tolerance.angle_frame = atan2(empty_goal_min_tolerance.frame_y, empty_goal_min_tolerance.frame_x) RAD2DEG;
        empty_goal_min_tolerance.dist_frame = sqrt(empty_goal_min_tolerance.frame_y * empty_goal_min_tolerance.frame_y + empty_goal_min_tolerance.frame_x * empty_goal_min_tolerance.frame_x);

        //====================================================================
        point2d_t temp_empty_goal_min_tolerance_fr;
        temp_empty_goal_min_tolerance_fr.x = empty_goal_min_tolerance.frame_x;
        temp_empty_goal_min_tolerance_fr.y = empty_goal_min_tolerance.frame_y;
        point2d_t temp_empty_goal_min_tolerance_world;
        polar2d_t temp_empty_goal_min_tolerance_world_polar;
        frameToWorld(temp_empty_goal_min_tolerance_fr, &temp_empty_goal_min_tolerance_world, &temp_empty_goal_min_tolerance_world_polar, &robot_pose, LUTfr2lap);
        empty_goal_min_tolerance.field_x = temp_empty_goal_min_tolerance_world.x;
        empty_goal_min_tolerance.field_y = temp_empty_goal_min_tolerance_world.y;
        empty_goal_min_tolerance.dist_field = temp_empty_goal_min_tolerance_world_polar.r;
        empty_goal_min_tolerance.angle_field = temp_empty_goal_min_tolerance_world_polar.theta;
        //====================================================================

        empty_goal_max_tolerance.frame_x = x_kanan_offset - center_cam_x;
        empty_goal_max_tolerance.frame_y = center_cam_y - y_kanan_offset;
        empty_goal_max_tolerance.angle_frame = atan2(empty_goal_max_tolerance.frame_y, empty_goal_max_tolerance.frame_x) RAD2DEG;
        empty_goal_max_tolerance.dist_frame = sqrt(empty_goal_max_tolerance.frame_y * empty_goal_max_tolerance.frame_y + empty_goal_max_tolerance.frame_x * empty_goal_max_tolerance.frame_x);

        //====================================================================
        point2d_t temp_empty_goal_max_tolerance_fr;
        temp_empty_goal_max_tolerance_fr.x = empty_goal_max_tolerance.frame_x;
        temp_empty_goal_max_tolerance_fr.y = empty_goal_max_tolerance.frame_y;
        point2d_t temp_empty_goal_max_tolerance_world;
        polar2d_t temp_empty_goal_max_tolerance_world_polar;
        frameToWorld(temp_empty_goal_max_tolerance_fr, &temp_empty_goal_max_tolerance_world, &temp_empty_goal_max_tolerance_world_polar, &robot_pose, LUTfr2lap);
        empty_goal_max_tolerance.field_x = temp_empty_goal_max_tolerance_world.x;
        empty_goal_max_tolerance.field_y = temp_empty_goal_max_tolerance_world.y;
        empty_goal_max_tolerance.dist_field = temp_empty_goal_max_tolerance_world_polar.r;
        empty_goal_max_tolerance.angle_field = temp_empty_goal_max_tolerance_world_polar.theta;
        //====================================================================

        float x_kiri_cm = empty_goal_min_tolerance.field_x;
        float y_kiri_cm = empty_goal_min_tolerance.field_y;
        float x_kanan_cm = empty_goal_max_tolerance.field_x;
        float y_kanan_cm = empty_goal_max_tolerance.field_y;
        float x_tengah_cm = (x_kiri_cm + x_kanan_cm) * 0.5;
        float y_tengah_cm = (y_kiri_cm + y_kanan_cm) * 0.5;

        // printf("%.2f %.2f -> %.2f\n", x_kiri_cm, x_kanan_cm, x_tengah_cm);

        empty_goal.field_x = x_tengah_cm;
        empty_goal.field_y = 1200;
        // empty_goal.field_y = y_tengah_cm;
        frameFieldCaluculate(&empty_goal);

        rectangle(frame_goal_keeper_bgr, rect, Scalar(255, 0, 255), 2);
        circle(frame_goal_keeper_bgr, Point(x_kiri, y_kiri), 7, Scalar(255, 0, 0), -1);
        circle(frame_goal_keeper_bgr, Point(x_kanan, y_kanan), 7, Scalar(0, 255, 0), -1);
        circle(frame_goal_keeper_bgr, Point(x_kiri_offset, y_kiri_offset), 7, Scalar(255, 0, 255), -1);
        circle(frame_goal_keeper_bgr, Point(x_kanan_offset, y_kanan_offset), 7, Scalar(0, 255, 255), -1);
        circle(frame_goal_keeper_bgr, Point(empty_goal.frame_x, empty_goal.frame_y), 7, Scalar(255, 255, 0), -1);
        // circle(frame_goal_keeper_bgr, Point(frame_x_target_2024, frame_y_target_2024), 7, Scalar(0, 0, 0), -1);

        circle(frame_goal_keeper_bgr, Point(gawang_kiri.frame_x, gawang_kiri.frame_y), 5, Scalar(0, 0, 0), -1);
        circle(frame_goal_keeper_bgr, Point(gawang_kanan.frame_x, gawang_kanan.frame_y), 5, Scalar(0, 0, 0), -1);

        line(frame_goal_keeper_bgr, Point(center_cam_x, center_cam_y), Point(center_cam_x, 0), Scalar(0, 0, 255), 1);
        line(frame_goal_keeper_bgr, Point(center_cam_x, center_cam_y), Point(enemy_goal.frame_x, enemy_goal.frame_y), Scalar(255, 0, 0), 1);

        // circle(frame_goal_keeper_bgr, Point(enemy_goal.frame_x, enemy_goal.frame_y), 10, Scalar(255, 0, 0), -1);

        // printf("Hello: %d %d || %d %d\n", empty_goal.frame_x, empty_goal.frame_y, center_cam_x, center_cam_y);
        sudut_robot_ke_gawang_vision = atan2f(center_cam_y - frame_y_target_2024, frame_x_target_2024 - center_cam_x) RAD2DEG;

        is_goal_empty = 1;

        float error_sudut_ke_tengah = atan2f(center_cam_y - enemy_goal.frame_y, enemy_goal.frame_x - center_cam_x) RAD2DEG - sudut_robot_ke_gawang_vision;

        while (error_sudut_ke_tengah < -180)
            error_sudut_ke_tengah += 360;
        while (error_sudut_ke_tengah > 180)
            error_sudut_ke_tengah -= 360;

        if (error_sudut_ke_tengah < 0) {
            int x_offset = (x_kiri - frame_x_target_2024) * 0.15;
            lock_goal_kiri_atau_kanan = 1;
            sudut_robot_ke_gawang_vision = atan2f(center_cam_y - frame_y_target_2024, (frame_x_target_2024 + x_offset) - center_cam_x) RAD2DEG;
            circle(frame_goal_keeper_bgr, Point((frame_x_target_2024 + x_offset), frame_y_target_2024), 7, Scalar(0, 0, 0), -1);
        } else {
            int x_offset = (x_kanan - frame_x_target_2024) * 0.15;
            lock_goal_kiri_atau_kanan = 2;
            sudut_robot_ke_gawang_vision = atan2f(center_cam_y - frame_y_target_2024, (frame_x_target_2024 + x_offset) - center_cam_x) RAD2DEG;
            circle(frame_goal_keeper_bgr, Point((frame_x_target_2024 + x_offset), frame_y_target_2024), 7, Scalar(0, 0, 0), -1);
        }
    } else {
        is_goal_empty = 0;
    }

    // Transmit All
    iris_msgs::goalpost msg_goalpost;
    msg_goalpost.keeper_on_field_x = real_keeper_on_field[0];
    msg_goalpost.keeper_on_field_y = real_keeper_on_field[1];

    msg_goalpost.empty_goal_field_x = empty_goal.field_x;
    msg_goalpost.empty_goal_field_y = empty_goal.field_y;
    msg_goalpost.empty_goal_field_theta = empty_goal.angle_field;
    msg_goalpost.empty_goal_frame_x = empty_goal.frame_x;
    msg_goalpost.empty_goal_frame_y = empty_goal.frame_y;
    msg_goalpost.empty_goal_frame_theta = empty_goal.angle_frame;
    msg_goalpost.empty_goal_min_tolerance_x = empty_goal_min_tolerance.field_x;
    msg_goalpost.empty_goal_min_tolerance_y = empty_goal_min_tolerance.field_y;
    msg_goalpost.empty_goal_max_tolerance_x = empty_goal_max_tolerance.field_x;
    msg_goalpost.empty_goal_max_tolerance_y = empty_goal_max_tolerance.field_y;
    msg_goalpost.sudut_robot_ke_gawang_vision = sudut_robot_ke_gawang_vision;
    msg_goalpost.lock_goal_kiri_atau_kanan = lock_goal_kiri_atau_kanan;

    msg_goalpost.empty_goal_status = is_goal_empty;

    pub_goalpost.publish(msg_goalpost);

    //--Publish
    static uint8_t cnt = 0;

    if (cnt >= 6) {
        sensor_msgs::ImagePtr msg_pub_frame_goalpost = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame_goal_keeper_bgr).toImageMsg();
        pub_frame_goalpost.publish(msg_pub_frame_goalpost);
        sensor_msgs::ImagePtr msg_pub_frame_goalpost_debug = cv_bridge::CvImage(std_msgs::Header(), "mono8", frame_binary_empty_goal).toImageMsg();
        pub_frame_goalpost_binary.publish(msg_pub_frame_goalpost_debug);
    }
    cnt++;
}

void callbackSubcribeRobotPose(const geometry_msgs::Pose2DConstPtr& msg)
{
    robot_pose.x = msg->x;
    robot_pose.y = msg->y;
    robot_pose.theta = msg->theta;
}

void calculateKeeperPosition(point2d_t px_fr, float real_keeper_on_field[2])
{
    float keeper_on_center_frame[3];
    keeper_on_center_frame[0] = px_fr.x - center_cam_x;
    keeper_on_center_frame[1] = center_cam_y - px_fr.y;
    keeper_on_center_frame[2] = atan2(keeper_on_center_frame[1], keeper_on_center_frame[0]) RAD2DEG;

    float keeper_dist_on_frame = sqrt(keeper_on_center_frame[0] * keeper_on_center_frame[0] + keeper_on_center_frame[1] * keeper_on_center_frame[1]);

    float keeper_theta_on_field = keeper_on_center_frame[2] + robot_pose.theta - 90;
    while (keeper_on_center_frame[2] < 0)
        keeper_on_center_frame[2] += 360;
    while (keeper_on_center_frame[2] >= 360)
        keeper_on_center_frame[2] -= 360;

    float keeper_dist_on_field = LUTfr2lap[(int)keeper_on_center_frame[2] * 3200 + (int)(keeper_dist_on_frame * 10)]; // versi 2
    while (keeper_theta_on_field < -180)
        keeper_theta_on_field += 360;
    while (keeper_theta_on_field > 180)
        keeper_theta_on_field -= 360;

    real_keeper_on_field[0] = robot_pose.x + keeper_dist_on_field * cos(keeper_theta_on_field DEG2RAD);
    real_keeper_on_field[1] = robot_pose.y + keeper_dist_on_field * sin(keeper_theta_on_field DEG2RAD);
}

void field2FrameScanning(Point_frame_field* point_struct, int tolerance, uint8_t* keeper_found, const Scalar& color, Mat* frame_binary_keeper_obs, Mat* frame_roi_bgr, float real_keeper_on_field[2])
{
    for (float angle = 0; angle < 360; angle += 2.5) {
        if (*keeper_found > 0)
            return;

        int px = 0;
        point2d_t px_fr;
        px_fr.x = point_struct->frame_x + px * cosf(angle DEG2RAD);
        px_fr.y = point_struct->frame_y - px * sinf(angle DEG2RAD);

        while (1) {
            if (px > tolerance)
                break;
            else if (frame_binary_keeper_obs->at<unsigned char>(Point(px_fr.x, px_fr.y)) == 255) {
                circle(*frame_roi_bgr, Point(px_fr.x, px_fr.y), 7, color, -1);
                calculateKeeperPosition(px_fr, real_keeper_on_field);
                *keeper_found = 1;
                return;
            }

            px++;
            px_fr.x = point_struct->frame_x + px * cosf(angle DEG2RAD);
            px_fr.y = point_struct->frame_y - px * sinf(angle DEG2RAD);
        }
    }
}

void frameFieldCaluculate(Point_frame_field* point_struct)
{
    point_struct->dist_field = sqrtf((point_struct->field_x - robot_pose.x) * (point_struct->field_x - robot_pose.x) + (point_struct->field_y - robot_pose.y) * (point_struct->field_y - robot_pose.y));
    point_struct->angle_field = atan2(point_struct->field_y - robot_pose.y, point_struct->field_x - robot_pose.x) RAD2DEG;
    if (point_struct->angle_field < 0)
        point_struct->angle_field += 360;
    else if (point_struct->angle_field > 360)
        point_struct->angle_field -= 360;

    // printf("%.2f || %.2f || %.2f || %.2f || %.2f \n",point_struct->angle_field,point_struct->dist_field,robot_pose.x,robot_pose.y,robot_pose.theta);

    point_struct->dist_frame = LUTlap2fr[(int)point_struct->angle_field * 1200 + (int)point_struct->dist_field] * 0.1;
    point_struct->frame_x = point_struct->dist_frame * cosf((point_struct->angle_field - robot_pose.theta + 90) DEG2RAD) + center_cam_x;
    point_struct->frame_y = center_cam_y - point_struct->dist_frame * sinf((point_struct->angle_field - robot_pose.theta + 90) DEG2RAD);
}