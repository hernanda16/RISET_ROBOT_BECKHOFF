#include "world_model/line_icp.hpp"

sensor_msgs::PointCloud line_reference_msg;
#define USE_NATIONAL

int main(int argc, char **argv)
{
    ros::init(argc, argv, "line_node");
    ros::NodeHandle nh;
    ros::MultiThreadedSpinner spinner(0);

    sub_robot_pose = nh.subscribe<geometry_msgs::Pose2D>("/world_model/robot/pose", 1, callbackSubcribeRobotPose);
    sub_vision_line_data = nh.subscribe("/vision/data/line", 1, callbackSubcribeVisionDataLine);
    pub_world_line = nh.advertise<sensor_msgs::PointCloud>("/world_model/line/pos", 1);
    pub_line_reference = nh.advertise<sensor_msgs::PointCloud>("/world_model/line/reference", 1);
    pub_d_icp = nh.advertise<geometry_msgs::Pose2D>("/world_model/line/d_icp", 1);
    pub_icp_score = nh.advertise<std_msgs::Float32>("/world_model/line/icp_score", 1);
    tim_routine = nh.createTimer(ros::Duration(0.02), callbackRoutine);

    loadConfig();
    loadLUT(LUTfr2lap);

#ifdef USE_NATIONAL
    create_line_ref_icp();
#else
    // create_line_ref_icp_regional();
    create_line_ref_icp_regional_real();
#endif

    spinner.spin();
    return 0;
}

void callbackRoutine(const ros::TimerEvent &event)
{
    // pub_line_reference.publish(line_reference_msg);
}

void callbackSubcribeVisionDataLine(const sensor_msgs::PointCloudConstPtr &msg)
{
    sensor_msgs::PointCloud line_world_msg;
    detection_line.clear();
    detection_line_in_world.clear();
    size_t total_line_points = msg->points.size();
    // printf("=================================================== %d\n", total_line_points);
    for (uint16_t i = 0; i < total_line_points; i++)
    {
        point2d_t temp;
        temp.x = msg->points[i].x;
        temp.y = msg->points[i].y;

        detection_line.push_back(temp);

        point2d_t line_world_point;
        polar2d_t line_world_polar;
        frameToWorld(temp, &line_world_point, &line_world_polar, &robot_pose, LUTfr2lap);

        if (line_world_polar.r > line_config.min_scan_line && line_world_polar.r < line_config.max_scan_line /*&& isInsideField(line_world_point)*/)
        {
            // printf("Line: %.f %.f %.f %.f\n", line_world_point.x, line_world_point.y, line_world_polar.r, line_world_polar.theta);
            geometry_msgs::Point32 temp_point;
            temp_point.x = line_world_point.x;
            temp_point.y = line_world_point.y;

            line_world_msg.points.push_back(temp_point);
            detection_line_in_world.push_back(line_world_point);
        }
    }

    // Publish
    pub_world_line.publish(line_world_msg);
}

void callbackSubcribeRobotPose(const geometry_msgs::Pose2DConstPtr &msg)
{
    // printf("HALO:\n");
    robot_pose.x = msg->x;
    robot_pose.y = msg->y;
    robot_pose.theta = msg->theta;

    static pose2d_t last_pose_icp_update = {0};

    // ROS_INFO("%.f %.f\n", px2cm_x(cm2px_x(0)), px2cm_y(cm2px_y(0)));

    // printf("Line Size: %d %d\n", detection_line_in_world.size(), line_config.min_line_points_found_threshold);

    if (detection_line_in_world.size() < line_config.min_line_points_found_threshold)
        return;

    float pos_diff = pythagoras(robot_pose.x, robot_pose.y, last_pose_icp_update.x, last_pose_icp_update.y);

    // printf("Pos Diff: %.f\n", pos_diff);

    if (pos_diff > line_config.pos_diff)
    {
        last_pose_icp_update.x = robot_pose.x;
        last_pose_icp_update.y = robot_pose.y;

        /* Create a new instance of Point Cloud */
        pcl::PointCloud<pcl::PointXYZ>::Ptr points_measurement(new pcl::PointCloud<pcl::PointXYZ>);
        pcl::PointCloud<pcl::PointXYZ>::Ptr points_reference(new pcl::PointCloud<pcl::PointXYZ>);

        /* Creating a point cloud from measured line points */
        for (int i = 0; i < detection_line_in_world.size(); i++)
        {
            pcl::PointXYZ point;
            point.x = detection_line_in_world[i].x;
            point.y = detection_line_in_world[i].y;
            point.z = 0;

            points_measurement->points.push_back(point);
        }

        /* Creating a point cloud from reference line points */
        for (int i = 0; i < reference_line_in_world.size(); i++)
        {
            pcl::PointXYZ point;
            point.x = reference_line_in_world[i].x;
            point.y = reference_line_in_world[i].y;
            point.z = 0;

            points_reference->points.push_back(point);
        }

        /* Set ICP algorithm parameters */
        pcl::IterativeClosestPoint<pcl::PointXYZ, pcl::PointXYZ> icp;
        icp.setInputSource(points_measurement);
        icp.setInputTarget(points_reference);
        icp.setMaximumIterations(line_config.icp_max_iterations);
        icp.align(points_final_fit);

        /**
         * Get ICP score,
         * Smaller ICP score indicate that ICP is converged the measured line points from its reference
         */
        icp_score = icp.getFitnessScore();

        /* Publish ICP score */
        std_msgs::Float32 msg_icp_score;
        msg_icp_score.data = icp_score;
        pub_icp_score.publish(msg_icp_score);

        /**
         * Get final matrix transformation, this matrix is used to transfrom measured points to reference points,
         * so we use this transformation to transform our robot pose (from odometry) into new pose based on ICP
         */
        Eigen::Matrix4f transformation_matrix = icp.getFinalTransformation();

        /* Get Robot's translation and rotation based on ICP */
        Eigen::Vector4f point(last_pose_icp_update.x, last_pose_icp_update.y, 0, 1);
        Eigen::Vector4f transformed_point = transformation_matrix * point;
        dth_icp = atan2(transformation_matrix(1, 0), transformation_matrix(0, 0)) * 180 / M_PI;
        dx_icp = transformed_point[0] - last_pose_icp_update.x;
        dy_icp = transformed_point[1] - last_pose_icp_update.y;
        if (dth_icp > 180)
            dth_icp -= 360;
        else if (dth_icp < -180)
            dth_icp += 360;

        /**
         * Send it to pose_estimator based on icp_score
         * If icp_score is more than threshold_icp_score, it indicates that ICP cannot converged measured points to refenrece,
         * so we conclude that ICP is false, and so we con't use it (see Callback from pose_estimator)
         */
        uint8_t icp_score_status = (icp_score <= line_config.threshold_icp_score);
        // printf("ICP Score: %d %.f %.f\n", icp_score_status, icp_score, line_config.threshold_icp_score);
        if (icp_score_status)
        {
            geometry_msgs::Pose2D msg_d_icp;
            msg_d_icp.x = dx_icp;
            msg_d_icp.y = dy_icp;
            msg_d_icp.theta = dth_icp;
            pub_d_icp.publish(msg_d_icp);
        }
    }
}

void create_line_ref_icp()
{
    using namespace cv;
    Mat frame_lapangan_raw = Mat::zeros(1300, 900, CV_8UC3);

    rectangle(frame_lapangan_raw, Rect(Point(50, 50), Point(850, 1250)), Scalar(255, 255, 255), 3);

    ellipse(frame_lapangan_raw, Point(50, 50), Size(50, 50), 180, 270, 180, Scalar(255, 255, 255), 3, 0);
    ellipse(frame_lapangan_raw, Point(850, 50), Size(50, 50), 180, 270, 360, Scalar(255, 255, 255), 3, 0);
    rectangle(frame_lapangan_raw, Rect(Point(200, 50), Point(700, 230)), Scalar(255, 255, 255), 3);
    rectangle(frame_lapangan_raw, Rect(Point(300, 50), Point(600, 100)), Scalar(255, 255, 255), 3);

    line(frame_lapangan_raw, Point(50, 650), Point(850, 650), Scalar(255, 255, 255), 3);
    circle(frame_lapangan_raw, Point(450, 650), 130, Scalar(255, 255, 255), 3);

    rectangle(frame_lapangan_raw, Rect(Point(200, 1250), Point(700, 1250 - 180)), Scalar(255, 255, 255), 3);
    rectangle(frame_lapangan_raw, Rect(Point(300, 1250), Point(600, 1200)), Scalar(255, 255, 255), 3);
    ellipse(frame_lapangan_raw, Point(50, 1250), Size(50, 50), 180, 180, 90, Scalar(255, 255, 255), 3, 0);
    ellipse(frame_lapangan_raw, Point(850, 1250), Size(50, 50), 180, 90, 0, Scalar(255, 255, 255), 3, 0);

    reference_line_in_world.clear();
    for (uint16_t x = 0; x < frame_lapangan_raw.cols; x += 10)
    {
        for (uint16_t y = 0; y < frame_lapangan_raw.rows; y += 5)
        {
            if (frame_lapangan_raw.at<Vec3b>(y, x) == Vec3b(255, 255, 255))
            {
                point2d_t temp;
                temp.x = px2cm_x(x);
                temp.y = px2cm_y(y);

                reference_line_in_world.push_back(temp);

                geometry_msgs::Point32 temp_point;
                temp_point.x = temp.x;
                temp_point.y = temp.y;
                line_reference_msg.points.push_back(temp_point);
            }
        }
    }
}

void create_line_ref_icp_regional()
{
    using namespace cv;
    Mat frame_lapangan_raw = Mat::zeros(1200, 800 + 110 + 100, CV_8UC3);

    rectangle(frame_lapangan_raw, Rect(Point((100 + -100), (0)), Point((800 + 100 + 110), (1200))), Scalar(255, 255, 255), 3);

    rectangle(frame_lapangan_raw, Rect(Point((100 + 0 + 10), (700 - 0)), Point((100 + 800 + 10), (700 - 600))), Scalar(255, 255, 255), 3);

    rectangle(frame_lapangan_raw, Rect(Point((100 + 200 + 10), (700 - 500 + 50)), Point((100 + 600 + 10), (700 - 600))), Scalar(255, 255, 255), 3);

    // rectangle(frame_lapangan_raw, Rect(Point((100 + -100 + 10), (700 - 300 + 150)), Point((100 + -50 + 10), (700 - 300 - 150))), Scalar(255, 255, 255), 3);

    // ellipse(frame_lapangan_raw, Point((100 + -100), (700 - 700)), Size(50, 50), 0, 0, 90, Scalar(255, 255, 255), 3, 0);
    // ellipse(frame_lapangan_raw, Point((100 + -100), (700 + 100)), Size(50, 50), 0, 0, 360, Scalar(255, 255, 255), 3, 0);

    ellipse(frame_lapangan_raw, Point((100 + 800 + 10), (700 - 600)), Size(50, 50), 180, -90, 0, Scalar(255, 255, 255), 3, 0);
    ellipse(frame_lapangan_raw, Point((100 + 0 + 10), (700 - 600)), Size(50, 50), 0, 90, 0, Scalar(255, 255, 255), 3, 0);

    rectangle(frame_lapangan_raw, Rect(Point((100 + 0 + 10), (700 - 200 + (75 / 2))), Point((100 + 75 + 10), (700 - 200 - (75 / 2)))), Scalar(255, 255, 255), 3);
    rectangle(frame_lapangan_raw, Rect(Point((100 + 800 + 10), (700 - 200 + (75 / 2))), Point((100 + 800 - 75 + 10), (700 - 200 - (75 / 2)))), Scalar(255, 255, 255), 3);

    line(frame_lapangan_raw, Point((100 + 75 + 10), (700 - 200)), Point((100 + 300 + 10), (700 - 200)), Scalar(255, 255, 255), 3);
    line(frame_lapangan_raw, Point((100 + 725 + 10), (700 - 200)), Point((100 + 500 + 10), (700 - 200)), Scalar(255, 255, 255), 3);

    line(frame_lapangan_raw, Point((100 + 800 + 120 + 10), (700 - 50)), Point((100 + 800 + 120 + 10), (700 - 550)), Scalar(255, 255, 255), 3);

    line(frame_lapangan_raw, Point((100 + 300 + 10), (700 - 0)), Point((100 + 300 + 10), (700 - 500 + 50)), Scalar(255, 255, 255), 3);
    line(frame_lapangan_raw, Point((100 + 500 + 10), (700 - 0)), Point((100 + 500 + 10), (700 - 500 + 50)), Scalar(255, 255, 255), 3);

    line(frame_lapangan_raw, Point((100 + 10), (700)), Point((100 + 10), (1200)), Scalar(255, 255, 255), 3);
    line(frame_lapangan_raw, Point((100 + 10 + 800), (700)), Point((100 + 10 + 800), (1200)), Scalar(255, 255, 255), 3);

    ellipse(frame_lapangan_raw, Point((100 + 400 + 10), (700 - 0)), Size(100, 100), 0, -180, 0, Scalar(255, 255, 255), 3, 0);
    ellipse(frame_lapangan_raw, Point((100 + 400 + 10), (700 - 0)), Size(130, 130), 0, 180, 0, Scalar(255, 255, 255), 3, 0);

    reference_line_in_world.clear();
    for (uint16_t x = 0; x < frame_lapangan_raw.cols; x += 10)
    {
        for (uint16_t y = 0; y < frame_lapangan_raw.rows; y += 5)
        {
            if (frame_lapangan_raw.at<Vec3b>(y, x) == Vec3b(255, 255, 255))
            {
                point2d_t temp;
                temp.x = px2cm_x(x);
                temp.y = px2cm_y(y);

                reference_line_in_world.push_back(temp);

                geometry_msgs::Point32 temp_point;
                temp_point.x = temp.x;
                temp_point.y = temp.y;
                line_reference_msg.points.push_back(temp_point);
            }
        }
    }

    // ROS_ERROR("%.f %.f\n", px2cm_x(400), px2cm_y(700));
    // ROS_ERROR("%.f %.f\n", px2cm_x(700), px2cm_y(250));
    // ROS_ERROR("%.f %.f\n", px2cm_x(900), px2cm_y(100));
    // ROS_ERROR("%.f %.f\n", px2cm_x(300), px2cm_y(250));

    // imshow("tes", frame_lapangan_raw);
    // waitKey(0);
}

void create_line_ref_icp_regional_real()
{
    using namespace cv;
    Mat frame_lapangan_raw = Mat::zeros(1400, 800 + 110 + 200, CV_8UC3);

    rectangle(frame_lapangan_raw, Rect(Point((100 + -90), (0)), Point((910 + 110 + 0), (1400))), Scalar(255, 255, 255), 3); // LUAR

    rectangle(frame_lapangan_raw, Rect(Point((100 + 0 + 10), (700 - 0)), Point((100 + 800 + 10), (700 - 600))), Scalar(255, 255, 255), 3);

    rectangle(frame_lapangan_raw, Rect(Point((100 + 200 + 10), (700 - 500 + 50)), Point((100 + 600 + 10), (700 - 600))), Scalar(255, 255, 255), 3);

    // rectangle(frame_lapangan_raw, Rect(Point((100 + -100 + 10), (700 - 300 + 150)), Point((100 + -50 + 10), (700 - 300 - 150))), Scalar(255, 255, 255), 3);

    // ellipse(frame_lapangan_raw, Point((100 + -100), (700 - 700)), Size(50, 50), 0, 0, 90, Scalar(255, 255, 255), 3, 0);
    // ellipse(frame_lapangan_raw, Point((100 + -100), (700 + 100)), Size(50, 50), 0, 0, 360, Scalar(255, 255, 255), 3, 0);

    ellipse(frame_lapangan_raw, Point((100 + 800 + 10), (700 - 600)), Size(50, 50), 180, -90, 0, Scalar(255, 255, 255), 3, 0);
    ellipse(frame_lapangan_raw, Point((100 + 0 + 10), (700 - 600)), Size(50, 50), 0, 90, 0, Scalar(255, 255, 255), 3, 0);

    rectangle(frame_lapangan_raw, Rect(Point((100 + 0 + 10), (700 - 200 + (75 / 2))), Point((100 + 75 + 10), (700 - 200 - (75 / 2)))), Scalar(255, 255, 255), 3);
    rectangle(frame_lapangan_raw, Rect(Point((100 + 800 + 10), (700 - 200 + (75 / 2))), Point((100 + 800 - 75 + 10), (700 - 200 - (75 / 2)))), Scalar(255, 255, 255), 3);

    line(frame_lapangan_raw, Point((100 + 75 + 10), (700 - 200)), Point((100 + 300 + 10), (700 - 200)), Scalar(255, 255, 255), 3);
    line(frame_lapangan_raw, Point((100 + 725 + 10), (700 - 200)), Point((100 + 500 + 10), (700 - 200)), Scalar(255, 255, 255), 3);

    line(frame_lapangan_raw, Point((100 + 800 + 120 + 10), (700 - 50)), Point((100 + 800 + 120 + 10), (700 - 550)), Scalar(255, 255, 255), 3);

    line(frame_lapangan_raw, Point((100 + 300 + 10), (700 - 0)), Point((100 + 300 + 10), (700 - 500 + 50)), Scalar(255, 255, 255), 3);
    line(frame_lapangan_raw, Point((100 + 500 + 10), (700 - 0)), Point((100 + 500 + 10), (700 - 500 + 50)), Scalar(255, 255, 255), 3);

    line(frame_lapangan_raw, Point((100 + 10), (700)), Point((100 + 10), (1300)), Scalar(255, 255, 255), 3);
    line(frame_lapangan_raw, Point((100 + 10 + 800), (700)), Point((100 + 10 + 800), (1300)), Scalar(255, 255, 255), 3);
    line(frame_lapangan_raw, Point((110), (1300)), Point((910), (1300)), Scalar(255, 255, 255), 3);

    ellipse(frame_lapangan_raw, Point((100 + 400 + 10), (700 - 0)), Size(100, 100), 0, -180, 0, Scalar(255, 255, 255), 3, 0);
    ellipse(frame_lapangan_raw, Point((100 + 400 + 10), (700 - 0)), Size(130, 130), 0, 180, 0, Scalar(255, 255, 255), 3, 0);

    rectangle(frame_lapangan_raw, Rect(Point(200 + 50 + 10, 1250 + 50), Point(700 + 50 + 10, 50 + 1250 - 180)), Scalar(255, 255, 255), 3);
    rectangle(frame_lapangan_raw, Rect(Point(300 + 50 + 10, 1250 + 50), Point(600 + 50 + 10, 50 + 1200)), Scalar(255, 255, 255), 3);
    ellipse(frame_lapangan_raw, Point(50 + 10 + 50, 1250 + 50), Size(50, 50), 180, 180, 90, Scalar(255, 255, 255), 3, 0);
    ellipse(frame_lapangan_raw, Point(50 + 10 + 850, 1250 + 50), Size(50, 50), 180, 90, 0, Scalar(255, 255, 255), 3, 0);

    line(frame_lapangan_raw, Point((110), (100 + 150)), Point((800 + 110), (100 + 150)), Scalar(255, 255, 255), 3);

    reference_line_in_world.clear();
    for (uint16_t x = 0; x < frame_lapangan_raw.cols; x += 10) // 10
    {
        for (uint16_t y = 0; y < frame_lapangan_raw.rows; y += 5) // 5
        {
            if (frame_lapangan_raw.at<Vec3b>(y, x) == Vec3b(255, 255, 255))
            {
                point2d_t temp;
                temp.x = px2cm_x(x);
                temp.y = px2cm_y(y);

                reference_line_in_world.push_back(temp);

                // printf("%d %d: %.2f %.2f\n", x, y, temp.x, temp.y);

                geometry_msgs::Point32 temp_point;
                temp_point.x = temp.x;
                temp_point.y = temp.y;
                line_reference_msg.points.push_back(temp_point);
            }
        }
    }

    // printf("==============================\n");

    // ROS_ERROR("%.f %.f\n", px2cm_x(400), px2cm_y(700));
    // ROS_ERROR("%.f %.f\n", px2cm_x(700), px2cm_y(250));
    // ROS_ERROR("%.f %.f\n", px2cm_x(900), px2cm_y(100));
    // ROS_ERROR("%.f %.f\n", px2cm_x(300), px2cm_y(250));

    // flip(frame_lapangan_raw, frame_lapangan_raw, 0);

    // imshow("tes", frame_lapangan_raw);
    // waitKey(0);
}

// Transformer
// 1200 x 800
float cm2px_x(float x)
{
#ifdef USE_NATIONAL
    return (50 + x);
#else
    return (910 - x);
#endif
}
float cm2px_y(float y)
{
#ifdef USE_NATIONAL
    return (1250 - y);
#else
    return (100 + y);
#endif
}
float px2cm_x(float x)
{
#ifdef USE_NATIONAL
    return (x - 50);
#else
    return (x - 110);
#endif
}
float px2cm_y(float y)
{
#ifdef USE_NATIONAL
    return (1250 - y);
#else
    return (y - 100);
#endif
}

void loadConfig()
{
    char *robot_num;
    robot_num = getenv("ROBOT");
    char config_file[100], config_file_2[100];
    std::string current_dir = ros::package::getPath("world_model");
    sprintf(config_file, "%s/../../config/IRIS%s/static_conf.yaml", current_dir.c_str(), robot_num);

    YAML::Node config = YAML::LoadFile(config_file);
    line_config.min_scan_line = config["Line_ICP"]["min_scan_line"].as<int16_t>();
    line_config.max_scan_line = config["Line_ICP"]["max_scan_line"].as<int16_t>();
    line_config.min_line_points_found_threshold = config["Line_ICP"]["min_line_points_found_threshold"].as<int16_t>();
    line_config.threshold_icp_score = config["Line_ICP"]["threshold_icp_score"].as<int16_t>();
    line_config.pos_diff = config["Line_ICP"]["pos_diff"].as<int16_t>();
    line_config.icp_max_iterations = config["Line_ICP"]["icp_max_iterations"].as<int16_t>();
}
