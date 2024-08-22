#include "world_model/obs.hpp"

int main(int argc, char** argv)
{
    ros::init(argc, argv, "obs_node");
    ros::NodeHandle nh;
    ros::MultiThreadedSpinner spinner(0);

    sub_vision_obs_data = nh.subscribe("/vision/data/obs", 1, callbackSubcribeVisionDataObs);
    sub_robot_pose = nh.subscribe<geometry_msgs::Pose2D>("/world_model/robot/pose", 1, callbackSubcribeRobotPose);
    sub_shared_data = nh.subscribe<iris_msgs::data_robots>("/communication/icast/data_robot", 1, callbackSubcribeSharedData);

    pub_obstacle = nh.advertise<sensor_msgs::PointCloud>("/world_model/obs/pos", 1);
    pub_obstacle_pcl = nh.advertise<sensor_msgs::PointCloud>("/world_model/obs/pcl", 1);
    pub_obstacle_shared_all = nh.advertise<sensor_msgs::PointCloud>("/world_model/obs/shared_all", 1);
    tim_routine = nh.createTimer(ros::Duration(0.02), callbackRoutine);

    loadConfig();
    loadLUT(LUTfr2lap);

    spinner.spin();
    return 0;
}

void callbackRoutine(const ros::TimerEvent& event)
{
    mtx_obs.lock();
    std::vector<point2d_t> obs_merged = fuseObsLocal();
    mtx_obs.unlock();

    std::vector<point2d_t> obs_shared_all = mergeSharedObs(obs_merged);
    // printf("ASYDASD\n");
    // std::vector<point2d_t> obs_shared_all_filt = filterAllObs(obs_shared_all);

    // std::sort(obs_shared_all.begin(), obs_shared_all.end(), [](const point2d_t &a, const point2d_t &b)
    //           { return pythagoras(a.x, a.y, 0, 0) < pythagoras(b.x, b.y, 0, 0); });

    sensor_msgs::PointCloud msg_obs_merged;
    size_t obs_merged_size = obs_merged.size();
    for (size_t i = 0; i < obs_merged_size; i++) {
        geometry_msgs::Point32 temp_point;
        temp_point.x = obs_merged[i].x;
        temp_point.y = obs_merged[i].y;
        msg_obs_merged.points.push_back(temp_point);
    }
    pub_obstacle.publish(msg_obs_merged);

    sensor_msgs::PointCloud msg_obs_shared_all;
    size_t obs_shared_all_size = obs_shared_all.size();
    for (size_t i = 0; i < obs_shared_all_size; i++) {
        geometry_msgs::Point32 temp_point;
        temp_point.x = obs_shared_all[i].x;
        temp_point.y = obs_shared_all[i].y;
        msg_obs_shared_all.points.push_back(temp_point);
    }
    pub_obstacle_shared_all.publish(msg_obs_shared_all);
}

void callbackSubcribeRobotPose(const geometry_msgs::Pose2DConstPtr& msg)
{
    robot_pose.x = msg->x;
    robot_pose.y = msg->y;
    robot_pose.theta = msg->theta;
}

void callbackSubcribeVisionDataObs(const sensor_msgs::PointCloudConstPtr& msg)
{
    mtx_obs.lock();
    obs.clear();
    my_obs_on_field.clear();
    sensor_msgs::PointCloud msg_obs_pcl;
    size_t total_obs_points = msg->points.size();
    for (uint16_t i = 0; i < total_obs_points; i++) {
        point2d_t temp;
        temp.x = msg->points[i].x;
        temp.y = msg->points[i].y;
        obs.push_back(temp);

        if (pythagoras(temp.x, temp.y, 0, 0) > 200) {
            continue;
        }

        point2d_t temp_world;
        polar2d_t temp_polar_world;
        frameToWorld(temp, &temp_world, &temp_polar_world, &robot_pose, LUTfr2lap);

        if (temp_polar_world.r > obs_config.min_scan_obs && temp_polar_world.r < obs_config.max_scan_obs && isInsideField(temp_world)) {
            my_obs_on_field.push_back(temp_world);
            geometry_msgs::Point32 temp_point;
            temp_point.x = temp_world.x;
            temp_point.y = temp_world.y;
            msg_obs_pcl.points.push_back(temp_point);
        }
    }
    pub_obstacle_pcl.publish(msg_obs_pcl);
    mtx_obs.unlock();
}

std::vector<point2d_t> filterAllObs(std::vector<point2d_t> obs_shared)
{
    static std::vector<point2d_t> prev_obs_shared_all;

    printf("Befor: %d\n", prev_obs_shared_all.size());
    if (prev_obs_shared_all.size() == 0) {
        prev_obs_shared_all = obs_shared;
        return obs_shared;
    }

    printf("Hello: %d\n", prev_obs_shared_all.size());
    std::vector<point2d_t> obs_filtered;
    std::vector<point_2_point_associative_t> buffer;
    static std::vector<point_2_point_associative_t> prev_buffer;
    static std::vector<point_2_point_associative_t> buffer_buffer;

    // Nearest neighbour
    for (size_t i = 0; i < prev_obs_shared_all.size(); i++) {
        point_2_point_associative_t temp;
        temp.point_src.x = prev_obs_shared_all[i].x;
        temp.point_src.y = prev_obs_shared_all[i].y;
        temp.assosiated = 0;
        temp.dist_to_zero = pythagoras(temp.point_src.x, temp.point_src.y, 0, 0);
        temp.counter_hilang = 0;

        // Mencari titik terdekat dari obs[time_now]
        float min_dist = FLT_MAX;
        for (size_t j = 0; j < obs_shared.size(); j++) {
            float jarak_antar_titik = pythagoras(temp.point_src.x, temp.point_src.y, obs_shared[j].x, obs_shared[j].y);
            if (jarak_antar_titik < min_dist) {
                min_dist = jarak_antar_titik;
                temp.point_dst.x = obs_shared[j].x;
                temp.point_dst.y = obs_shared[j].y;
            }
        }

        // Jika ditemukan pasangannya
        if (min_dist < 80) {
            temp.assosiated = 1;
        }
        printf("TEST: %.2f %.2f %.2f %.2f %d\n", temp.point_src.x, temp.point_src.y, temp.point_dst.x, temp.point_dst.y, temp.assosiated);

        buffer.push_back(temp);
    }

    printf("ASDASD: %d\n", buffer.size());

    // Mengurutkan dari index terkecil dimulai dari paling dekat 0,0
    std::sort(buffer.begin(), buffer.end(), [](const point_2_point_associative_t& a, const point_2_point_associative_t& b) { return a.dist_to_zero < b.dist_to_zero; });

    if (prev_buffer.size() == 0) {
        prev_buffer = buffer;
        buffer_buffer = buffer;
        return obs_shared;
    }

    // for (size_t i = 0; i < buffer.size(); i++)
    // {
    //     float jarak = pythagoras(buffer[i].point_src.x,buffer[i].point_src.y,buffer_buffer[i].point_src.x,buffer_buffer[i].point_src.y);
    // }

    // Mengolah obstacle
    // for (size_t i = 0; i < buffer.size(); i++)
    // {
    //     if (buffer[i].assosiated == 0)
    //     {
    //         buffer[i].counter_hilang++;
    //     }

    //     if (buffer[i].counter_hilang < 50)
    //     {
    //         point2d_t temp;
    //         temp.x = buffer[i].point_src.x;
    //         temp.y = buffer[i].point_src.y;
    //         obs_filtered.push_back(temp);
    //     }
    // }

    printf("SIAE: %d\n", obs_filtered.size());

    prev_buffer = buffer;

    prev_obs_shared_all = obs_shared;

    return obs_filtered;
}

void callbackSubcribeSharedData(const iris_msgs::data_robotsConstPtr& msg)
{
    int8_t robot_num = atoi(getenv("ROBOT"));

    for (uint8_t i = 1; i < msg->pose_x.size(); i++) {
        shared_robot_data[i].pose.x = msg->pose_x.at(i);
    }
    for (uint8_t i = 1; i < msg->pose_y.size(); i++) {
        shared_robot_data[i].pose.y = msg->pose_y.at(i);
    }

    friends_obs_on_field.clear();

    if (robot_num != 1) {
        for (int8_t i = 0; i < msg->obs_pcl_x_1.size(); i++) {
            point2d_t temp_obs;
            temp_obs.x = msg->obs_pcl_x_1[i];
            temp_obs.y = msg->obs_pcl_y_1[i];
            friends_obs_on_field.push_back(temp_obs);
        }
    }

    if (robot_num != 2) {
        for (int8_t i = 0; i < msg->obs_pcl_x_2.size(); i++) {
            point2d_t temp_obs;
            temp_obs.x = msg->obs_pcl_x_2[i];
            temp_obs.y = msg->obs_pcl_y_2[i];
            friends_obs_on_field.push_back(temp_obs);
        }
    }

    if (robot_num != 3) {
        for (int8_t i = 0; i < msg->obs_pcl_x_3.size(); i++) {
            point2d_t temp_obs;
            temp_obs.x = msg->obs_pcl_x_3[i];
            temp_obs.y = msg->obs_pcl_y_3[i];
            friends_obs_on_field.push_back(temp_obs);
        }
    }

    // printf("BERAPA %d\n ", friends_obs_on_field.size());
}

std::vector<point2d_t> mergeSharedObs(std::vector<point2d_t> obs_local)
{
    std::vector<point2d_t> obs_merged;

    obs_merged.insert(obs_merged.end(), obs_local.begin(), obs_local.end());

    for (size_t i = 0; i < friends_obs_on_field.size(); i++) {
        // Jika robot mati
        // if (shared_robot_data[i].pose.x < FLT_EPSILON && shared_robot_data[i].pose.y < FLT_EPSILON)
        //     continue;

        // Filter from local obs
        uint8_t is_valid = 1;
        for (size_t j = 0; j < obs_local.size(); j++) {
            float dist = pythagoras(friends_obs_on_field[i], obs_local[j]);
            if (dist < k_thresshold) {
                is_valid = 0;
            }
        }

        if (is_valid == 0)
            continue;

        // Filter from robots
        for (size_t j = 1; j < 4; j++) {
            // Jika robot mati
            if (shared_robot_data[j].pose.x < FLT_EPSILON && shared_robot_data[j].pose.y < FLT_EPSILON)
                continue;

            float dist_to_robot = pythagoras(friends_obs_on_field[i].x, friends_obs_on_field[i].y, shared_robot_data[j].pose.x, shared_robot_data[j].pose.y);

            // printf("%d %d (%.2f %.2f) (%.2f %.2f) %.2f\n", i, j, friends_obs_on_field[i].x, friends_obs_on_field[i].y, shared_robot_data[j].pose.x, shared_robot_data[j].pose.y, dist_to_robot);
            if (/*all_robot_state[j] == 1 &&*/ dist_to_robot < 100) {
                is_valid = 0;
                // printf("DIHAPUS::\n");
            }
        }
        // printf("VALID: %d -> %.2f %.2f\n", is_valid, friends_obs_on_field[i].x, friends_obs_on_field[i].y);

        if (is_valid == 0)
            continue;

        obs_merged.push_back(friends_obs_on_field[i]);
    }

    return obs_merged;
}

std::vector<point2d_t> fuseObsLocal()
{
    std::vector<point2d_t> ret_temp;

    size_t my_obs_on_field_size = my_obs_on_field.size();

    uint8_t point_visited[my_obs_on_field_size];
    memset(point_visited, 0, my_obs_on_field_size);
    for (size_t i = 0; i < my_obs_on_field_size; i++) {
        if (point_visited[i] == 1)
            continue;

        // Filter dari robot
        uint8_t is_valid = 1;
        for (size_t j = 1; j < 4; j++) {
            if (shared_robot_data[j].pose.x < FLT_EPSILON && shared_robot_data[j].pose.y < FLT_EPSILON)
                continue;
            float dist_to_robot = pythagoras(my_obs_on_field[i].x, my_obs_on_field[i].y, shared_robot_data[j].pose.x, shared_robot_data[j].pose.y);
            if (dist_to_robot < 100) {
                is_valid = 0;
            }
        }

        if (is_valid == 0)
            continue;

        point2d_t child_points;
        size_t total_child_point;
        std::vector<point2d_t> child_points_group;
        child_points.x = 0;
        child_points.y = 0;
        total_child_point = 0;
        child_points_group.push_back(my_obs_on_field[i]);
        for (size_t j = 0; j < my_obs_on_field_size; j++) {
            for (size_t k = 0; k < child_points_group.size(); k++) {
                if (point_visited[j] == 1)
                    break;

                float dist = pythagoras(child_points_group[k], my_obs_on_field[j]);

                if (dist < k_thresshold) {
                    child_points.x += my_obs_on_field[j].x;
                    child_points.y += my_obs_on_field[j].y;
                    total_child_point++;
                    point_visited[j] = 1;

                    child_points_group.push_back(my_obs_on_field[j]);
                }
            }
        }

        if (total_child_point > minimum_child_points_to_be_grouped) {
            point2d_t temp;
            temp.x = child_points.x / total_child_point;
            temp.y = child_points.y / total_child_point;

            ret_temp.push_back(temp);
        }
    }

    return ret_temp;
}

std::vector<point2d_t> fuseObs()
{

    std::vector<point2d_t> ret_temp;
    std::vector<point2d_t> obs_on_field;

    obs_on_field.insert(obs_on_field.end(), my_obs_on_field.begin(), my_obs_on_field.end());
    obs_on_field.insert(obs_on_field.end(), friends_obs_on_field.begin(), friends_obs_on_field.end());

    size_t obs_on_field_size = obs_on_field.size();

    uint8_t point_visited[obs_on_field_size];
    memset(point_visited, 0, obs_on_field_size);
    for (size_t i = 0; i < obs_on_field_size; i++) {
        if (point_visited[i] == 1)
            continue;

        // Filter dari robot
        uint8_t is_valid = 1;
        for (size_t j = 0; j < 5; j++) {
            float dist_to_robot = pythagoras(obs_on_field[i], all_robot_pos[j]);
            if (all_robot_state[j] == 1 && dist_to_robot < k_thresshold) {
                is_valid = 0;
            }
        }

        if (is_valid == 0)
            continue;

        point2d_t child_points;
        size_t total_child_point;
        std::vector<point2d_t> child_points_group;
        child_points.x = 0;
        child_points.y = 0;
        total_child_point = 0;
        child_points_group.push_back(obs_on_field[i]);
        for (size_t j = 0; j < obs_on_field_size; j++) {
            for (size_t k = 0; k < child_points_group.size(); k++) {
                if (point_visited[j] == 1)
                    break;

                float dist = pythagoras(child_points_group[k], obs_on_field[j]);

                if (dist < k_thresshold) {
                    child_points.x += obs_on_field[j].x;
                    child_points.y += obs_on_field[j].y;
                    total_child_point++;
                    point_visited[j] = 1;

                    child_points_group.push_back(obs_on_field[j]);
                }
            }
        }

        if (total_child_point > minimum_child_points_to_be_grouped) {
            point2d_t temp;
            temp.x = child_points.x / total_child_point;
            temp.y = child_points.y / total_child_point;

            ret_temp.push_back(temp);
        }
    }

    return ret_temp;
}

void loadConfig()
{
    char* robot_num;
    robot_num = getenv("ROBOT");
    char config_file[100], config_file_2[100];
    std::string current_dir = ros::package::getPath("world_model");
    sprintf(config_file, "%s/../../config/IRIS%s/static_conf.yaml", current_dir.c_str(), robot_num);

    YAML::Node config = YAML::LoadFile(config_file);
    obs_config.min_scan_obs = config["Obstacle"]["min_scan_obs"].as<int16_t>();
    obs_config.max_scan_obs = config["Obstacle"]["max_scan_obs"].as<int16_t>();
}
