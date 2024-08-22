#include "vision/detection2.hpp"

int main(int argc, char** argv)
{
    ros::init(argc, argv, "detection_node");
    ros::NodeHandle nh;
    ros::MultiThreadedSpinner spinner(0);
    image_transport::ImageTransport IT(nh);

    loadConfig();

    sub_frame_bgr = IT.subscribe("/vision/frame_bgr", 1, callbackSubcribeFrameBGR);
    sub_control_box = nh.subscribe("/threshold_params", 1, callbackSubControlBox);

    pub_vision_ball_data = nh.advertise<sensor_msgs::PointCloud>("/vision/data/ball", 1);
    pub_vision_line_data = nh.advertise<sensor_msgs::PointCloud>("/vision/data/line", 1);
    pub_vision_obs_data = nh.advertise<sensor_msgs::PointCloud>("/vision/data/obs", 1);
    pub_vision_status = nh.advertise<iris_msgs::vision_status>("/vision/status", 1);

    pub_frame_display = IT.advertise("/vision/display", 1);
    pub_frame_yuv = IT.advertise("/vision/display/yuv", 1);
    pub_frame_ball = IT.advertise("/vision/display/ball", 1);
    pub_frame_ball_debug = IT.advertise("/vision/debug/ball", 1);
    pub_frame_line_debug = IT.advertise("/vision/debug/line", 1);
    pub_frame_line_pcl_debug = IT.advertise("/vision/debug/line_pcl", 1);
    pub_frame_obs_debug = IT.advertise("/vision/debug/obs", 1);
    pub_frame_field_debug = IT.advertise("/vision/debug/field", 1);

    tim_routine = nh.createTimer(ros::Duration(0.0166666666666), callbackRoutine);

    spinner.spin();

    return 0;
}

void callbackSubcribeFrameBGR(const sensor_msgs::ImageConstPtr& msg)
{
    mutex_frame_bgr.lock();
    try {
        frame_bgr = cv_bridge::toCvShare(msg, "bgr8")->image.clone();
        flag = 1;
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

void callbackRoutine(const ros::TimerEvent& event)
{
    if (flag == 0)
        return;

    mutex_frame_bgr.lock();
    frame_bgr_processed = frame_bgr.clone();
    flag = 0;
    mutex_frame_bgr.unlock();

    //=============================================
    /**--------------------------------------------
     *     Convert to Color Other Color Space
     --------------------------------------------*/
    cvtColor(frame_bgr_processed, frame_yuv_processed, CV_BGR2YUV);
    cvtColor(frame_bgr_processed, frame_hsv_processed, COLOR_BGR2HSV);
    cvtColor(frame_bgr_processed, frame_hls_processed, CV_BGR2HLS);
    //=============================================
    /**--------------------------------------------
     *               Thresholding
     --------------------------------------------*/
    inRange(frame_yuv_processed, Scalar(controlbox_data[6], controlbox_data[8], controlbox_data[10]), Scalar(controlbox_data[7], controlbox_data[9], controlbox_data[11]), threshold_ball);
    inRange(frame_hls_processed, Scalar(controlbox_data[12], controlbox_data[14], controlbox_data[16]), Scalar(controlbox_data[13], controlbox_data[15], controlbox_data[17]), threshold_line);
    inRange(frame_hsv_processed, Scalar(controlbox_data[0], controlbox_data[2], controlbox_data[4]), Scalar(controlbox_data[1], controlbox_data[3], controlbox_data[5]), threshold_field);
    //=============================================
    /**--------------------------------------------
     *                  Masking
     --------------------------------------------*/
    Mat camera_mask = Mat::zeros(frame_bgr_processed.size(), CV_8UC1);
    circle(camera_mask, Point(center_cam_x, center_cam_y), outer_mask_radius, Scalar(255), FILLED, LINE_8);
    circle(camera_mask, Point(center_cam_x, center_cam_y), inner_mask_radius + 1, Scalar(0), FILLED, LINE_8);

    bitwise_and(camera_mask, threshold_field, threshold_field);
    //=============================================
    /**--------------------------------------------
     *                   Hull
     --------------------------------------------*/
    {
        int largest_area = 0;
        int largest_contour_index = 0;

        vector<vector<Point>> contour_field; // list of contour points
        vector<Vec4i> hierarchy_field;
        findContours(threshold_field, contour_field, hierarchy_field, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

        vector<vector<Point>> hull(contour_field.size());
        for (int i = 0; i < contour_field.size(); i++) {
            convexHull(Mat(contour_field[i]), hull[i], false);
            if (contour_field[i].size() > largest_area) {
                largest_area = contour_field[i].size();
                largest_contour_index = i;
            }
        }

        Mat frame_field_largest_hull = Mat::zeros(frame_hsv_processed.size(), CV_8UC1);
        drawContours(frame_field_largest_hull, hull, largest_contour_index, Scalar(255), -1, 8, vector<Vec4i>(), 0, Point());

        frame_field_hull = Mat::zeros(frame_hsv_processed.size(), CV_8UC1);
        for (int i = 0; i < contour_field.size(); i++)
            drawContours(frame_field_hull, hull, i, Scalar(255), -1, 8, vector<Vec4i>(), 0, Point());

        bitwise_or(frame_field_hull, frame_field_largest_hull, frame_field_hull);
    }
    //=============================================
    /**--------------------------------------------
     *                 Obstacle
     --------------------------------------------*/
    Mat threshold_obs = Mat::zeros(frame_hsv_processed.size(), CV_8UC1);

    Mat buffer_ball = threshold_ball.clone();
    dilate(buffer_ball, buffer_ball, getStructuringElement(MORPH_ELLIPSE, Size(6, 6)));
    bitwise_or(buffer_ball, threshold_field, threshold_field);

    bitwise_or(threshold_obs, threshold_field, threshold_obs);
    bitwise_or(threshold_obs, buffer_ball, threshold_obs);
    bitwise_or(threshold_obs, threshold_line, threshold_obs);

    dilate(threshold_obs, threshold_obs, getStructuringElement(MORPH_ELLIPSE, Size(4, 4)));

    bitwise_not(threshold_obs, threshold_obs);

    erode(threshold_obs, threshold_obs, getStructuringElement(MORPH_ELLIPSE, Size(8, 8)));
    dilate(threshold_obs, threshold_obs, getStructuringElement(MORPH_ELLIPSE, Size(6, 6)));

    bitwise_and(camera_mask, threshold_obs, threshold_obs);

    Mat obs_buffer = Mat::zeros(frame_hsv_processed.size(), CV_8UC3);

    // draw white pixel in obs to buffer with color of red
    for (int i = 0; i < threshold_obs.rows; i++) {
        for (int j = 0; j < threshold_obs.cols; j++) {
            if (threshold_obs.at<uint8_t>(i, j) == 255) {
                obs_buffer.at<Vec3b>(i, j)[0] = 0;
                obs_buffer.at<Vec3b>(i, j)[1] = 0;
                obs_buffer.at<Vec3b>(i, j)[2] = 255;
            }
        }
    }
    //=============================================
    /**--------------------------------------------
     *                   Hull
     --------------------------------------------*/
    {
        int largest_area = 0;
        int largest_area2 = 0;
        int largest_contour_index = 0;
        int largest_contour_index2 = 0;

        dilate(threshold_line, threshold_line, getStructuringElement(MORPH_ELLIPSE, Size(4, 4)));
        erode(threshold_line, threshold_line, getStructuringElement(MORPH_ELLIPSE, Size(4, 4)));

        dilate(threshold_ball, threshold_ball, getStructuringElement(MORPH_ELLIPSE, Size(4, 4)));
        erode(threshold_ball, threshold_ball, getStructuringElement(MORPH_ELLIPSE, Size(4, 4)));

        Mat field_buffer = threshold_field.clone();

        dilate(field_buffer, field_buffer, getStructuringElement(MORPH_ELLIPSE, Size(8, 8)));

        bitwise_or(field_buffer, threshold_line, field_buffer);
        bitwise_or(field_buffer, threshold_ball, field_buffer);
        bitwise_and(field_buffer, frame_field_hull, field_buffer);

        vector<vector<Point>> contour_field;
        vector<Vec4i> hierarchy_field;
        findContours(field_buffer, contour_field, hierarchy_field, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

        vector<vector<Point>> hull(contour_field.size());
        for (int i = 0; i < contour_field.size(); i++) {
            convexHull(Mat(contour_field[i]), hull[i], false);
        }

        frame_field_hull = Mat::zeros(frame_hsv_processed.size(), CV_8UC1);
        if (!hull.empty() && hull.size() > 1) {
            // Calculate areas of hulls and the average area
            vector<double> hullAreas(hull.size());
            double totalArea = 0;
            for (size_t i = 0; i < hull.size(); i++) {
                double area = contourArea(hull[i]);
                hullAreas[i] = area;
                totalArea += area;
            }

            double min_threshold = totalArea / hull.size();

            // Filter out hulls below average area
            vector<vector<Point>> filteredHulls;
            for (size_t i = 0; i < hull.size(); i++) {
                if (hullAreas[i] >= min_threshold) {
                    filteredHulls.push_back(hull[i]);
                }
            }

            for (int i = 0; i < filteredHulls.size(); i++) {
                drawContours(frame_field_hull, filteredHulls, i, Scalar(255), -1, 8, vector<Vec4i>(), 0, Point());
            }
        } else {
            for (int i = 0; i < hull.size(); i++) {
                drawContours(frame_field_hull, hull, i, Scalar(255), -1, 8, vector<Vec4i>(), 0, Point());
            }
        }
        dilate(frame_field_hull, frame_field_hull, getStructuringElement(MORPH_ELLIPSE, Size(4, 4)));
    }
    //=============================================
    /**--------------------------------------------
     *                  Masking
     --------------------------------------------*/
    bitwise_and(camera_mask, threshold_ball, threshold_ball);
    bitwise_and(camera_mask, threshold_line, threshold_line);
    // bitwise_and(camera_mask, frame_field_hull, frame_field_hull);
    bitwise_and(camera_mask, threshold_obs, threshold_obs);
    //=============================================
    /**--------------------------------------------
     *                  Draw Line
     --------------------------------------------*/
    frame_bgr_displayed = frame_bgr_processed.clone();
    line(frame_bgr_displayed, Point(center_cam_x, 0), Point(center_cam_x, 720), Scalar(255, 0, 255), 1, LINE_8);
    line(frame_bgr_displayed, Point(0, center_cam_y), Point(1280, center_cam_y), Scalar(255, 0, 255), 1, LINE_8);
    circle(frame_bgr_displayed, Point(center_cam_x, center_cam_y), outer_mask_radius, Scalar(255, 0, 255), 1, LINE_8);
    circle(frame_bgr_displayed, Point(center_cam_x, center_cam_y), inner_mask_radius, Scalar(255, 0, 255), 1, LINE_8);

    frame_ball_displayed = frame_bgr_displayed.clone();
    // frame_field_displayed = frame_bgr_displayed.clone();
    //=============================================
    /**--------------------------------------------
     *                   Ball
     --------------------------------------------*/
    bitwise_and(threshold_ball, frame_field_hull, threshold_ball);
    erode(threshold_ball, threshold_ball, getStructuringElement(MORPH_ELLIPSE, Size(4, 4)));
    dilate(threshold_ball, threshold_ball, getStructuringElement(MORPH_ELLIPSE, Size(4, 4)));
    //=============================================
    /**--------------------------------------------
     *             Ball Processing
     --------------------------------------------*/
    iris_msgs::vision_status vision_status_msg;
    sensor_msgs::PointCloud vision_ball_msg;

    vector<Point> center_balls;
    findBallCenter(&center_balls, threshold_ball);

    static float bola_x_buffer;
    static float bola_y_buffer;
    static uint64_t delay_in = 0;
    static uint64_t delay_out = 0;

    vision_ball_msg.points.reserve(center_balls.size());
    vision_ball_msg.points.resize(center_balls.size());

    if (center_balls.size() > 0) {
        bola_x_buffer = center_balls[0].x;
        bola_y_buffer = center_balls[0].y;

        delay_in++;
        delay_out = 0;
    } else {
        delay_in = 0;
        delay_out++;
    }

    if (delay_in > 1) {
        vision_status_msg.is_ball_seen = true;
    }

    if (delay_out > 17) {
        vision_status_msg.is_ball_seen = false;
    } else {
        vision_status_msg.is_ball_seen = true;

        vision_ball_msg.points.reserve(1);
        vision_ball_msg.points.resize(1);

        vision_ball_msg.points[0].x = bola_x_buffer;
        vision_ball_msg.points[0].y = bola_y_buffer;
    }

    if (center_balls.size() > 0) {
        circle(frame_bgr_displayed, Point(center_balls[0].x + center_cam_x, center_cam_y - center_balls[0].y), 5, Scalar(255, 0, 0), -1);
        // circle(frame_ball_displayed, Point(center_balls[0].x + center_cam_x, center_cam_y - center_balls[0].y), 5, Scalar(255, 0, 0), -1);
    }
    cvtColor(threshold_ball, threshold_ball, CV_GRAY2BGR);
    addWeighted(frame_ball_displayed, 1, threshold_ball, 0.2, 0, frame_ball_displayed);
    cvtColor(threshold_ball, threshold_ball, CV_BGR2GRAY);
    //=============================================
    /**--------------------------------------------
     *             Line Processing
     --------------------------------------------*/
    sensor_msgs::PointCloud vision_line_msg;

    vector<Point> pcl_line;
    findPointCloud(&pcl_line, threshold_line);

    vision_line_msg.points.reserve(pcl_line.size());
    vision_line_msg.points.resize(pcl_line.size());

    {
        for (uint16_t i = 0; i < pcl_line.size(); i++) {
            vision_line_msg.points[i].x = pcl_line[i].x;
            vision_line_msg.points[i].y = pcl_line[i].y;
        }
    }

    {
        frame_line_pcl = Mat::zeros(frame_hls_processed.size(), CV_8UC1);
        for (uint16_t i = 0; i < pcl_line.size(); i++) {
            circle(frame_line_pcl, Point(pcl_line[i].x + center_cam_x, center_cam_y - pcl_line[i].y), 2, Scalar(255), 2, LINE_8);
        }
    }
    //=============================================
    /**--------------------------------------------
     *              Obs Processing
     --------------------------------------------*/
    sensor_msgs::PointCloud vision_obs_msg;

    vector<Point> pcl_obs;
    findPointCloud(&pcl_obs, threshold_obs);

    vision_obs_msg.points.reserve(pcl_obs.size());
    vision_obs_msg.points.resize(pcl_obs.size());
    {
        for (uint16_t i = 0; i < pcl_obs.size(); i++) {
            vision_obs_msg.points[i].x = pcl_obs[i].x;
            vision_obs_msg.points[i].y = pcl_obs[i].y;
        }
    }
    {
        Mat pcl_obs_frame = Mat::zeros(frame_hsv_processed.size(), CV_8UC3);
        for (uint16_t i = 0; i < pcl_obs.size(); i++) {
            circle(pcl_obs_frame, Point(pcl_obs[i].x + center_cam_x, center_cam_y - pcl_obs[i].y), 2, Scalar(0, 0, 255), 2, LINE_8);
        }

        addWeighted(frame_bgr_displayed, 1, pcl_obs_frame, 0.6, 0, frame_bgr_displayed);
        addWeighted(frame_ball_displayed, 1, pcl_obs_frame, 0.6, 0, frame_ball_displayed);
    }
    {
        Mat edge_field = Mat::zeros(frame_hsv_processed.size(), CV_8UC3);
        vector<Point> pcl_edge;
        findPointCloud(&pcl_edge, frame_field_hull, 0);
        for (uint16_t i = 0; i < pcl_edge.size(); i++) {
            circle(edge_field, Point(pcl_edge[i].x + center_cam_x, center_cam_y - pcl_edge[i].y), 1, Scalar(255, 255, 0), 2, LINE_8);
        }
        addWeighted(frame_bgr_displayed, 1, edge_field, 0.4, 0, frame_bgr_displayed);
        addWeighted(frame_ball_displayed, 1, edge_field, 0.4, 0, frame_ball_displayed);
    }

    bitwise_and(threshold_obs, frame_field_hull, threshold_obs);
    cvtColor(frame_field_hull, frame_field_hull, CV_GRAY2BGR);
    bitwise_and(obs_buffer, frame_field_hull, obs_buffer);
    cvtColor(frame_field_hull, frame_field_hull, CV_BGR2GRAY);

    addWeighted(frame_bgr_displayed, 1, obs_buffer, 0.2, 0, frame_bgr_displayed);
    //=============================================
    /**--------------------------------------------
     *                Regression
     --------------------------------------------*/

#ifndef REGRESSION
#define REGRESSION
#endif
#ifdef REGRESSION
    {
        Mat line_regression = Mat::zeros(frame_bgr_displayed.size(), CV_8UC3);
        for (float angle = 0; angle < 360; angle += 10) {
            int16_t pixel_x = center_cam_x + 180 * cos(angle DEG2RAD);
            int16_t pixel_y = center_cam_y - 180 * sin(angle DEG2RAD);
            line(line_regression, Point(center_cam_x, center_cam_y), Point(pixel_x, pixel_y), Scalar(255, 0, 255, 255), 1, LINE_8);
        }
        addWeighted(frame_bgr_displayed, 1, line_regression, 0.8, 0, frame_bgr_displayed);
    }
#endif
    //=============================================
    /**--------------------------------------------
     *                  Publish
     --------------------------------------------*/
    pub_vision_ball_data.publish(vision_ball_msg);
    pub_vision_status.publish(vision_status_msg);
    pub_vision_line_data.publish(vision_line_msg);
    pub_vision_obs_data.publish(vision_obs_msg);

    static uint8_t cnt = 0;
    if (cnt++ < 5)
        return;

    cnt = 0;

    //----Field
    sensor_msgs::ImagePtr msg_pub_frame_display = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame_bgr_displayed).toImageMsg();
    pub_frame_display.publish(msg_pub_frame_display);
    sensor_msgs::ImagePtr msg_pub_frame_obs_debug = cv_bridge::CvImage(std_msgs::Header(), "mono8", frame_field_hull).toImageMsg();
    pub_frame_obs_debug.publish(msg_pub_frame_obs_debug);
    sensor_msgs::ImagePtr msg_pub_frame_field_debug = cv_bridge::CvImage(std_msgs::Header(), "mono8", threshold_field).toImageMsg();
    pub_frame_field_debug.publish(msg_pub_frame_field_debug);

    //----Ball
    sensor_msgs::ImagePtr msg_pub_frame_ball = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame_ball_displayed).toImageMsg();
    pub_frame_ball.publish(msg_pub_frame_ball);
    sensor_msgs::ImagePtr msg_pub_frame_ball_debug = cv_bridge::CvImage(std_msgs::Header(), "mono8", threshold_ball).toImageMsg();
    pub_frame_ball_debug.publish(msg_pub_frame_ball_debug);
    sensor_msgs::ImagePtr msg_pub_frame_yuv = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame_yuv_processed).toImageMsg();
    pub_frame_yuv.publish(msg_pub_frame_yuv);

    //----Line
    sensor_msgs::ImagePtr msg_pub_frame_line_debug = cv_bridge::CvImage(std_msgs::Header(), "mono8", threshold_line).toImageMsg();
    pub_frame_line_debug.publish(msg_pub_frame_line_debug);
    sensor_msgs::ImagePtr msg_pub_frame_line_pcl_debug = cv_bridge::CvImage(std_msgs::Header(), "mono8", frame_line_pcl).toImageMsg();
    pub_frame_line_pcl_debug.publish(msg_pub_frame_line_pcl_debug);
    //=============================================
}

void findBallCenter(vector<Point>* center_ptr, Mat mask)
{
    center_ptr->clear();
    vector<vector<Point>> contours;
    findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    std::sort(contours.begin(), contours.end(), [](const auto& c1, const auto& c2) {
        double area1 = cv::contourArea(c1);
        double area2 = cv::contourArea(c2);
        return area1 > area2; // Sort in descending order
    });

    for (size_t i = 0; i < contours.size(); i++) {
        Point2f center;
        float radius;
        minEnclosingCircle(contours[i], center, radius);
        Moments m = moments(contours[i]);
        // Point center_point(m.m10 / m.m00 - center_cam_x, center_cam_y - m.m01 / m.m00);

        // Tambahan
        if (m.m00 == 0)
            continue;
        float center_ball_x = m.m10 / m.m00;
        float center_ball_y = m.m01 / m.m00;

        Point new_center;
        new_center.x = center_ball_x; // Untuk safety
        new_center.y = center_ball_y; // Untuk safety

        int dx = center_cam_x - center_ball_x;
        int dy = center_cam_y - center_ball_y;
        float angle_to_center = atan2(dy, dx) * 180 / M_PI;
        float distance_to_center = sqrt(dx * dx + dy * dy);

        // Scan dari center bola ke center robot untuk mencari titik terluar bola yang paling dekat dengan robot
        for (int pixel = 0; pixel < distance_to_center; pixel++) {
            int x = center.x + pixel * cos(angle_to_center DEG2RAD);
            int y = center.y + pixel * sin(angle_to_center DEG2RAD);
            if (mask.at<uint8_t>(y, x) == 0) {
                new_center.x = x;
                new_center.y = y;
                break;
            }
        }

        // Mengembalikan agar titik pusat bola berada di tengah
        new_center.x = new_center.x - center_cam_x;
        new_center.y = center_cam_y - new_center.y;

        if (i < 5)
            center_ptr->push_back(new_center);
    }
}

void findPointCloud(vector<Point>* line_ptr, Mat mask, uint8_t is_rising)
{
    Mat frame = Mat::zeros(mask.size(), CV_8UC1);
    Mat combine = Mat::zeros(mask.size(), CV_8UC1);
    Mat mask_clone = mask.clone();
    uint8_t pixel_value = 0;

    uint16_t last_pixel_checked[2] = { center_cam_x, center_cam_y };

    line_ptr->clear();
    for (float angle = 0; angle < 360; angle += 2.5) {
        for (float magnitude = inner_mask_radius + 1; magnitude < mask_clone.cols * 0.6; magnitude += 1) {
            uint16_t pixel_x = (center_cam_x + magnitude * cos(angle DEG2RAD)) + 1;
            uint16_t pixel_y = (center_cam_y - magnitude * sin(angle DEG2RAD)) + 1;

            //--Check if pixel is out of bounds
            if (pixel_x >= mask_clone.cols || pixel_x <= 0 || pixel_y >= mask_clone.rows || pixel_y <= 0) {
                break;
            }

            //--Check if pixel is already checked
            if (pixel_x == last_pixel_checked[0] && pixel_y == last_pixel_checked[1]) {
                continue;
            } else {
                last_pixel_checked[0] = pixel_x;
                last_pixel_checked[1] = pixel_y;
            }

            pixel_value = mask_clone.at<uint8_t>(pixel_y, pixel_x);

            if (is_rising) {
                //--Check if pixel is black or white
                if (pixel_value == 0) {
                    continue;
                } else if (pixel_value == 255) {
                    line_ptr->push_back(Point(pixel_x - center_cam_x, center_cam_y - pixel_y));
                    break;
                }
            } else {
                //--Check if pixel is black or white
                if (pixel_value == 255) {
                    continue;
                } else if (pixel_value == 0) {
                    line_ptr->push_back(Point(pixel_x - center_cam_x, center_cam_y - pixel_y));
                    break;
                }
            }
        }
    }
}

void loadConfig()
{
    char* robot_num;
    robot_num = getenv("ROBOT");
    char config_file[100];
    char config_file1[100];
    std::string current_dir = ros::package::getPath("vision");
    sprintf(config_file, "%s/../../config/IRIS%s/dynamic_conf.yaml", current_dir.c_str(), robot_num);
    sprintf(config_file1, "%s/../../config/IRIS%s/static_conf.yaml", current_dir.c_str(), robot_num);

    YAML::Node config = YAML::LoadFile(config_file);
    controlbox_data[0] = config["Vision"]["field_h_min"].as<int>();
    controlbox_data[1] = config["Vision"]["field_h_max"].as<int>();
    controlbox_data[2] = config["Vision"]["field_s_min"].as<int>();
    controlbox_data[3] = config["Vision"]["field_s_max"].as<int>();
    controlbox_data[4] = config["Vision"]["field_v_min"].as<int>();
    controlbox_data[5] = config["Vision"]["field_v_max"].as<int>();
    controlbox_data[6] = config["Vision"]["ball_y_min"].as<int>();
    controlbox_data[7] = config["Vision"]["ball_y_max"].as<int>();
    controlbox_data[8] = config["Vision"]["ball_u_min"].as<int>();
    controlbox_data[9] = config["Vision"]["ball_u_max"].as<int>();
    controlbox_data[10] = config["Vision"]["ball_v_min"].as<int>();
    controlbox_data[11] = config["Vision"]["ball_v_max"].as<int>();
    controlbox_data[12] = config["Vision"]["line_h_min"].as<int>();
    controlbox_data[13] = config["Vision"]["line_h_max"].as<int>();
    controlbox_data[14] = config["Vision"]["line_l_min"].as<int>();
    controlbox_data[15] = config["Vision"]["line_l_max"].as<int>();
    controlbox_data[16] = config["Vision"]["line_s_min"].as<int>();
    controlbox_data[17] = config["Vision"]["line_s_max"].as<int>();

    YAML::Node config1 = YAML::LoadFile(config_file1);
    center_cam_x = config1["Camera"]["cam_offset_x"].as<int>();
    center_cam_y = config1["Camera"]["cam_offset_y"].as<int>();
    outer_mask_radius = config1["Camera"]["outer_mask_radius"].as<int>();
    inner_mask_radius = config1["Camera"]["inner_mask_radius"].as<int>();

    center_cam_x += (int)(cam_res_x * 0.5);
    center_cam_y += (int)(cam_res_y * 0.5);
}

void saveConfig()
{
    try {
        char* robot_num = getenv("ROBOT");
        char config_file[100];
        std::string current_dir = ros::package::getPath("vision");
        sprintf(config_file, "%s/../../config/IRIS%s/dynamic_conf.yaml", current_dir.c_str(), robot_num);

        YAML::Node config = YAML::LoadFile(config_file);
        // config["Vision"]["field_h_min"] = to_string(controlbox_data[0]);
        // config["Vision"]["field_h_max"] = to_string(controlbox_data[1]);
        // config["Vision"]["field_s_min"] = to_string(controlbox_data[2]);
        // config["Vision"]["field_s_max"] = to_string(controlbox_data[3]);
        // config["Vision"]["field_v_min"] = to_string(controlbox_data[4]);
        // config["Vision"]["field_v_max"] = to_string(controlbox_data[5]);
        // config["Vision"]["ball_y_min"] = to_string(controlbox_data[6]);
        // config["Vision"]["ball_y_max"] = to_string(controlbox_data[7]);
        // config["Vision"]["ball_u_min"] = to_string(controlbox_data[8]);
        // config["Vision"]["ball_u_max"] = to_string(controlbox_data[9]);
        // config["Vision"]["ball_v_min"] = to_string(controlbox_data[10]);
        // config["Vision"]["ball_v_max"] = to_string(controlbox_data[11]);
        // config["Vision"]["line_h_min"] = to_string(controlbox_data[12]);
        // config["Vision"]["line_h_max"] = to_string(controlbox_data[13]);
        // config["Vision"]["line_l_min"] = to_string(controlbox_data[14]);
        // config["Vision"]["line_l_max"] = to_string(controlbox_data[15]);
        // config["Vision"]["line_s_min"] = to_string(controlbox_data[16]);
        // config["Vision"]["line_s_max"] = to_string(controlbox_data[17]);

        std::ofstream fout(config_file);
        fout << config;
        fout.close();
    } catch (const std::exception& e) {
        ROS_ERROR("SAVE CAMERA CONFIG %s", e.what());
    }
}
