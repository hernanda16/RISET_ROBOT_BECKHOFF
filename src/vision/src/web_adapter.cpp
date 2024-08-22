#include "iris_msgs/CamOffset.h"
#include "iris_msgs/YuvThresholdInit.h"
#include "ros/ros.h"
#include "std_msgs/Bool.h"
#include "std_msgs/Int16MultiArray.h"
#include "std_msgs/Int8.h"
#include "std_msgs/UInt16MultiArray.h"
#include "std_msgs/UInt32MultiArray.h"
#include "yaml-cpp/yaml.h"
#include <dirent.h>
#include <filesystem>
#include <fstream>
#include <ros/package.h>
#include <ros/param.h>
#include <sstream>
#include <stack>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <vector>

using namespace std;

// Subscribers
ros::Subscriber sub_yuv_field;
ros::Subscriber sub_yuv_ball;
ros::Subscriber sub_yuv_line;
ros::Subscriber sub_yuv_keeper_obs;
ros::Subscriber sub_lst_params;
ros::Subscriber sub_get_field_LUT_filename;
ros::Subscriber sub_get_ball_LUT_filename;
ros::Subscriber sub_get_line_LUT_filename;
ros::Subscriber sub_vision_field_switcher;
ros::Subscriber sub_vision_ball_switcher;
ros::Subscriber sub_vision_line_switcher;
ros::Subscriber sub_k_passing;

// Services
ros::ServiceServer ser_yuv_field;
ros::ServiceServer ser_yuv_ball;
ros::ServiceServer ser_yuv_line;
ros::ServiceServer ser_lst_params;
ros::ServiceServer ser_cam_offset;
ros::ServiceServer ser_yuv_keeper_obs;
ros::ServiceServer ser_data_toggle;
ros::ServiceServer ser_k_passing;

// Var
uint16_t obs_size_threshold;
uint16_t yuv_ball_thresh[6];
uint16_t yuv_field_thresh[6];
uint16_t yuv_line_thresh[6];
uint16_t yuv_keeper_obs[6];
uint16_t r_scan_min_cm = 70;
uint16_t r_scan_max_cm = 200;
std_msgs::UInt16MultiArray web_datas_msgs;
vector<double> regresi;
int8_t g_cam_offset_x;
int8_t g_cam_offset_y;
uint8_t r_cam;
uint16_t g_res_x = 360;
uint16_t g_res_y = 640;
int16_t g_center_cam_x = (int)(g_res_x * 0.5 + g_cam_offset_x);
int16_t g_center_cam_y = (int)(g_res_y * 0.5 + g_cam_offset_y);
int16_t vision_swithcer_datas[4] = { 0, 0, 0, 0 };
uint16_t k_passing[4];

// Publisher
ros::Publisher pub_web_datas;
ros::Publisher pub_ball_lut_name;
ros::Publisher pub_field_lut_name;
ros::Publisher pub_line_lut_name;
ros::Publisher pub_vision_toggle;

void SaveConfig(uint8_t var);
void LoadConfig();

void CllbckSubLstParams(const std_msgs::UInt16MultiArrayConstPtr& msg);
void CllbckSubYuvBall(const std_msgs::UInt16MultiArray::ConstPtr& msg);
void CllbckSubYuvField(const std_msgs::UInt16MultiArray::ConstPtr& msg);
void CllbckSubYuvLine(const std_msgs::UInt16MultiArray::ConstPtr& msg);
void CllbckSubYuvKeeperObs(const std_msgs::UInt16MultiArray::ConstPtr& msg);
void CallbackGetBallLUTName(const std_msgs::Bool& msg);
void CallbackGetLineLUTName(const std_msgs::Bool& msg);
void CallbackGetFieldLUTName(const std_msgs::Bool& msg);
void CllbckVisionFieldSwitcher(const std_msgs::Int8ConstPtr& msg);
void CllbckVisionBallSwitcher(const std_msgs::Int8ConstPtr& msg);
void CllbckVisionLineSwitcher(const std_msgs::Int8ConstPtr& msg);
void CllbckSubKPassing(const std_msgs::UInt16MultiArray::ConstPtr& msg);

bool CllbckSerYuvBall(iris_msgs::YuvThresholdInit::Request& req, iris_msgs::YuvThresholdInit::Response& res);
bool CllbckSerYuvField(iris_msgs::YuvThresholdInit::Request& req, iris_msgs::YuvThresholdInit::Response& res);
bool CllbckSerYuvLine(iris_msgs::YuvThresholdInit::Request& req, iris_msgs::YuvThresholdInit::Response& res);
bool CllbckSerYuvKeeperObs(iris_msgs::YuvThresholdInit::Request& req, iris_msgs::YuvThresholdInit::Response& res);
bool CllbckVisionToggle(iris_msgs::YuvThresholdInit::Request& req, iris_msgs::YuvThresholdInit::Response& res);
bool CllbckSerLSTParam(iris_msgs::YuvThresholdInit::Request& req, iris_msgs::YuvThresholdInit::Response& res);
bool CllbckSerCamOffset(iris_msgs::CamOffset::Request& req, iris_msgs::CamOffset::Response& res);
bool CllbckSerKPassing(iris_msgs::YuvThresholdInit::Request& req, iris_msgs::YuvThresholdInit::Response& res);

int main(int argc, char** argv)
{
    ros::init(argc, argv, "web_adapter");

    ros::NodeHandle NH;
    ros::MultiThreadedSpinner spinner(0);

    LoadConfig();

    sub_lst_params = NH.subscribe("lst_params", 1, CllbckSubLstParams);
    sub_yuv_field = NH.subscribe("yuv_field_thresh_final", 1, CllbckSubYuvField);
    sub_yuv_ball = NH.subscribe("yuv_ball_thresh_final", 1, CllbckSubYuvBall);
    sub_yuv_line = NH.subscribe("yuv_line_thresh_final", 1, CllbckSubYuvLine);
    sub_yuv_keeper_obs = NH.subscribe("yuv_keeper_obs_thresh_final", 1, CllbckSubYuvKeeperObs);
    sub_vision_field_switcher = NH.subscribe("vision_switcher_field", 1, CllbckVisionFieldSwitcher);
    sub_vision_ball_switcher = NH.subscribe("vision_switcher_ball", 1, CllbckVisionBallSwitcher);
    sub_vision_line_switcher = NH.subscribe("vision_switcher_line", 1, CllbckVisionLineSwitcher);
    sub_k_passing = NH.subscribe("k_passing", 1, CllbckSubKPassing);

    ser_cam_offset = NH.advertiseService("/cam_offset", CllbckSerCamOffset);
    ser_yuv_field = NH.advertiseService("/yuv_field_thresh_ser", CllbckSerYuvField);
    ser_yuv_ball = NH.advertiseService("/yuv_ball_thresh_ser", CllbckSerYuvBall);
    ser_yuv_line = NH.advertiseService("/yuv_line_thresh_ser", CllbckSerYuvLine);
    ser_yuv_keeper_obs = NH.advertiseService("yuv_keeper_obs_ser", CllbckSerYuvKeeperObs);
    ser_data_toggle = NH.advertiseService("vision_toggle", CllbckVisionToggle);
    ser_lst_params = NH.advertiseService("/lst_params_ser", CllbckSerLSTParam);
    ser_k_passing = NH.advertiseService("k_passing_ser", CllbckSerKPassing);

    pub_field_lut_name = NH.advertise<std_msgs::UInt32MultiArray>("vision_field_lut_name", 1);
    pub_ball_lut_name = NH.advertise<std_msgs::UInt32MultiArray>("vision_ball_lut_name", 1);
    pub_line_lut_name = NH.advertise<std_msgs::UInt32MultiArray>("vision_line_lut_name", 1);

    sub_get_field_LUT_filename = NH.subscribe("vision_lut/get_field_lut_filename", 1, CallbackGetFieldLUTName);
    sub_get_ball_LUT_filename = NH.subscribe("vision_lut/get_ball_lut_filename", 1, CallbackGetBallLUTName);
    sub_get_line_LUT_filename = NH.subscribe("vision_lut/get_line_lut_filename", 1, CallbackGetLineLUTName);

    pub_web_datas = NH.advertise<std_msgs::UInt16MultiArray>("threshold_params", 2);

    spinner.spin();

    return 0;
}

void CllbckSubKPassing(const std_msgs::UInt16MultiArray::ConstPtr& msg)
{
    web_datas_msgs.data.push_back(6);
    web_datas_msgs.data.push_back(4);
    for (uint8_t i = 0; i < 4; i++) {
        k_passing[i] = msg->data[i];
        web_datas_msgs.data.push_back(msg->data[i]);
    }

    pub_web_datas.publish(web_datas_msgs);
    web_datas_msgs.data.clear();
    SaveConfig(6);
}

void CllbckVisionFieldSwitcher(const std_msgs::Int8ConstPtr& msg)
{
    vision_swithcer_datas[0] = msg->data;

    ROS_INFO("data toggle field %d\n", msg->data);

    web_datas_msgs.data.push_back(5);
    web_datas_msgs.data.push_back(3);
    web_datas_msgs.data.push_back(vision_swithcer_datas[0]);
    web_datas_msgs.data.push_back(vision_swithcer_datas[1]);
    web_datas_msgs.data.push_back(vision_swithcer_datas[2]);

    pub_web_datas.publish(web_datas_msgs);
    web_datas_msgs.data.clear();

    SaveConfig(5);
}

bool CllbckSerCamOffset(iris_msgs::CamOffset::Request& req, iris_msgs::CamOffset::Response& res)
{
    try {
        res.data.push_back(g_cam_offset_x);
        res.data.push_back(g_cam_offset_y);
    } catch (const std::exception& e) {
        std::cerr << "test " << e.what() << '\n';
    }

    return true;
}

void CllbckVisionBallSwitcher(const std_msgs::Int8ConstPtr& msg)
{
    vision_swithcer_datas[1] = msg->data;

    ROS_INFO("data toggle ball %d\n", msg->data);

    web_datas_msgs.data.push_back(5);
    web_datas_msgs.data.push_back(3);
    web_datas_msgs.data.push_back(vision_swithcer_datas[0]);
    web_datas_msgs.data.push_back(vision_swithcer_datas[1]);
    web_datas_msgs.data.push_back(vision_swithcer_datas[2]);

    pub_web_datas.publish(web_datas_msgs);
    web_datas_msgs.data.clear();

    SaveConfig(5);
}

void CllbckVisionLineSwitcher(const std_msgs::Int8ConstPtr& msg)
{
    vision_swithcer_datas[2] = msg->data;

    ROS_INFO("data toggle line %d\n", msg->data);

    web_datas_msgs.data.push_back(5);
    web_datas_msgs.data.push_back(3);
    web_datas_msgs.data.push_back(vision_swithcer_datas[0]);
    web_datas_msgs.data.push_back(vision_swithcer_datas[1]);
    web_datas_msgs.data.push_back(vision_swithcer_datas[2]);

    pub_web_datas.publish(web_datas_msgs);
    web_datas_msgs.data.clear();

    SaveConfig(5);
}

void CllbckSubYuvBall(const std_msgs::UInt16MultiArray::ConstPtr& msg)
{
    web_datas_msgs.data.push_back(1);
    web_datas_msgs.data.push_back(6);
    for (uint8_t i = 0; i < 6; i++) {
        yuv_ball_thresh[i] = msg->data[i];
        web_datas_msgs.data.push_back(msg->data[i]);
    }

    pub_web_datas.publish(web_datas_msgs);
    web_datas_msgs.data.clear();

    SaveConfig(0);
}

void CllbckSubYuvField(const std_msgs::UInt16MultiArray::ConstPtr& msg)
{
    web_datas_msgs.data.push_back(0);
    web_datas_msgs.data.push_back(6);
    for (uint8_t i = 0; i < 6; i++) {
        yuv_field_thresh[i] = msg->data[i];
        web_datas_msgs.data.push_back(msg->data[i]);
    }

    pub_web_datas.publish(web_datas_msgs);
    web_datas_msgs.data.clear();

    SaveConfig(1);
}

void CllbckSubYuvLine(const std_msgs::UInt16MultiArray::ConstPtr& msg)
{
    web_datas_msgs.data.push_back(2);
    web_datas_msgs.data.push_back(6);
    for (uint8_t i = 0; i < 6; i++) {
        yuv_line_thresh[i] = msg->data[i];
        web_datas_msgs.data.push_back(msg->data[i]);
    }

    pub_web_datas.publish(web_datas_msgs);
    web_datas_msgs.data.clear();

    SaveConfig(2);
}

void CllbckSubYuvKeeperObs(const std_msgs::UInt16MultiArray::ConstPtr& msg)
{
    web_datas_msgs.data.push_back(3);
    web_datas_msgs.data.push_back(6);
    for (uint8_t i = 0; i < 6; i++) {
        yuv_keeper_obs[i] = msg->data[i];
        web_datas_msgs.data.push_back(msg->data[i]);
    }

    pub_web_datas.publish(web_datas_msgs);
    web_datas_msgs.data.clear();

    SaveConfig(4);
}

void CllbckSubLstParams(const std_msgs::UInt16MultiArrayConstPtr& msg)
{
    web_datas_msgs.data.push_back(4);
    web_datas_msgs.data.push_back(2);
    r_scan_min_cm = msg->data[0];
    r_scan_max_cm = msg->data[1];
    web_datas_msgs.data.push_back(msg->data[0]);
    web_datas_msgs.data.push_back(msg->data[1]);
    pub_web_datas.publish(web_datas_msgs);

    web_datas_msgs.data.clear();

    SaveConfig(3);
}

bool CllbckVisionToggle(iris_msgs::YuvThresholdInit::Request& req, iris_msgs::YuvThresholdInit::Response& res)
{
    res.yuv_threshold.push_back(vision_swithcer_datas[0]);
    res.yuv_threshold.push_back(vision_swithcer_datas[1]);
    res.yuv_threshold.push_back(vision_swithcer_datas[2]);
    return true;
}

void CallbackGetFieldLUTName(const std_msgs::Bool& msg)
{
    char* robot_num = getenv("ROBOT");
    std::string current_dir = ros::package::getPath("vision");
    char directory[100];

    sprintf(directory, "%s/../../config/IRIS%s/data_yuv/field", current_dir.c_str(), robot_num);

    DIR* dr;
    struct dirent* en;
    dr = opendir(directory);
    string line, word;

    vector<vector<int>> nama_file;
    vector<string> sub_nama_file;
    vector<int> nama_file_enc;
    stringstream nama_file_temp;

    //======================================
    std_msgs::UInt32MultiArray msg_file_lut_bola;
    //======================================

    if (dr) {
        while ((en = readdir(dr)) != NULL) {
            if (en->d_name[0] == 'l' && en->d_name[1] == 'u' && en->d_name[2] == 't') {
                int i;
                string s = "";
                for (i = 0; i < strlen(en->d_name); i++) {
                    s = s + en->d_name[i];
                }

                //======================================

                stringstream ss(s);
                sub_nama_file.push_back("1");

                while (getline(ss, word, '_')) {
                    int word_size = word.size();

                    if (word[0] != 'l' && word[1] != 'u' && word[2] != 't' && word_size != 4 && word[word_size - 1] != 'v') {
                        sub_nama_file.push_back(word);
                    }

                    if (word[word_size - 1] == 'v') {
                        stringstream sss(word);
                        string sub_word;

                        while (getline(sss, sub_word, '.')) {
                            if (sub_word[0] != 'c')
                                sub_nama_file.push_back(sub_word);
                        }
                    }
                }

                //======================================

                nama_file_temp.str("");
                for (int i = 0; i < sub_nama_file.size(); i++) {
                    nama_file_temp << sub_nama_file[i];
                }

                //======================================
                sub_nama_file.clear();
                //======================================

                int kirim;
                for (int i = 0; i < nama_file_temp.str().size(); i++) {
                    if (kirim != stoi(nama_file_temp.str())) {
                        kirim = stoi(nama_file_temp.str());
                        msg_file_lut_bola.data.push_back(stoi(nama_file_temp.str()));
                    }
                }

                //======================================
            }
        }

        pub_field_lut_name.publish(msg_file_lut_bola);
        closedir(dr);
    }
}

void CallbackGetBallLUTName(const std_msgs::Bool& msg)
{
    char* robot_num = getenv("ROBOT");
    std::string current_dir = ros::package::getPath("vision");
    char directory[100];

    sprintf(directory, "%s/../../config/IRIS%s/data_yuv/ball", current_dir.c_str(), robot_num);

    DIR* dr;
    struct dirent* en;
    dr = opendir(directory);
    string line, word;

    vector<vector<int>> nama_file;
    vector<string> sub_nama_file;
    vector<int> nama_file_enc;
    stringstream nama_file_temp;

    //======================================
    std_msgs::UInt32MultiArray msg_file_lut_bola;
    //======================================

    if (dr) {
        while ((en = readdir(dr)) != NULL) {
            if (en->d_name[0] == 'l' && en->d_name[1] == 'u' && en->d_name[2] == 't') {
                int i;
                string s = "";
                for (i = 0; i < strlen(en->d_name); i++) {
                    s = s + en->d_name[i];
                }

                //======================================

                stringstream ss(s);
                sub_nama_file.push_back("1");

                while (getline(ss, word, '_')) {
                    int word_size = word.size();

                    if (word[0] != 'l' && word[1] != 'u' && word[2] != 't' && word_size != 4 && word[word_size - 1] != 'v') {
                        sub_nama_file.push_back(word);
                    }

                    if (word[word_size - 1] == 'v') {
                        stringstream sss(word);
                        string sub_word;

                        while (getline(sss, sub_word, '.')) {
                            if (sub_word[0] != 'c')
                                sub_nama_file.push_back(sub_word);
                        }
                    }
                }

                //======================================

                nama_file_temp.str("");
                for (int i = 0; i < sub_nama_file.size(); i++) {
                    nama_file_temp << sub_nama_file[i];
                }

                //======================================
                sub_nama_file.clear();
                //======================================

                int kirim;
                for (int i = 0; i < nama_file_temp.str().size(); i++) {
                    if (kirim != stoi(nama_file_temp.str())) {
                        kirim = stoi(nama_file_temp.str());
                        msg_file_lut_bola.data.push_back(stoi(nama_file_temp.str()));
                    }
                }

                //======================================
            }
        }

        pub_ball_lut_name.publish(msg_file_lut_bola);
        closedir(dr);
    }
}

void CallbackGetLineLUTName(const std_msgs::Bool& msg)
{
    char* robot_num = getenv("ROBOT");
    std::string current_dir = ros::package::getPath("vision");
    char directory[100];

    sprintf(directory, "%s/../../config/IRIS%s/data_yuv/line", current_dir.c_str(), robot_num);

    DIR* dr;
    struct dirent* en;
    dr = opendir(directory);
    string line, word;

    vector<vector<int>> nama_file;
    vector<string> sub_nama_file;
    vector<int> nama_file_enc;
    stringstream nama_file_temp;

    //======================================
    std_msgs::UInt32MultiArray msg_file_lut_bola;
    //======================================

    if (dr) {
        while ((en = readdir(dr)) != NULL) {
            if (en->d_name[0] == 'l' && en->d_name[1] == 'u' && en->d_name[2] == 't') {
                int i;
                string s = "";
                for (i = 0; i < strlen(en->d_name); i++) {
                    s = s + en->d_name[i];
                }

                //======================================

                stringstream ss(s);
                sub_nama_file.push_back("1");

                while (getline(ss, word, '_')) {
                    int word_size = word.size();

                    if (word[0] != 'l' && word[1] != 'u' && word[2] != 't' && word_size != 4 && word[word_size - 1] != 'v') {
                        sub_nama_file.push_back(word);
                    }

                    if (word[word_size - 1] == 'v') {
                        stringstream sss(word);
                        string sub_word;

                        while (getline(sss, sub_word, '.')) {
                            if (sub_word[0] != 'c')
                                sub_nama_file.push_back(sub_word);
                        }
                    }
                }

                //======================================

                nama_file_temp.str("");
                for (int i = 0; i < sub_nama_file.size(); i++) {
                    nama_file_temp << sub_nama_file[i];
                }

                //======================================
                sub_nama_file.clear();
                //======================================

                int kirim;
                for (int i = 0; i < nama_file_temp.str().size(); i++) {
                    if (kirim != stoi(nama_file_temp.str())) {
                        kirim = stoi(nama_file_temp.str());
                        msg_file_lut_bola.data.push_back(stoi(nama_file_temp.str()));
                    }
                }

                //======================================
            }
        }

        pub_line_lut_name.publish(msg_file_lut_bola);
        closedir(dr);
    }
}

bool CllbckSerLSTParam(iris_msgs::YuvThresholdInit::Request& req, iris_msgs::YuvThresholdInit::Response& res)
{
    res.yuv_threshold.push_back(r_scan_min_cm);
    res.yuv_threshold.push_back(r_scan_max_cm);

    return true;
}

bool CllbckSerKPassing(iris_msgs::YuvThresholdInit::Request& req, iris_msgs::YuvThresholdInit::Response& res)
{
    for (uint8_t i = 0; i < 4; i++)
        res.yuv_threshold.push_back(k_passing[i]);
    return true;
}

bool CllbckSerYuvBall(iris_msgs::YuvThresholdInit::Request& req, iris_msgs::YuvThresholdInit::Response& res)
{
    for (uint8_t i = 0; i < 6; i++)
        res.yuv_threshold.push_back(yuv_ball_thresh[i]);
    return true;
}

bool CllbckSerYuvField(iris_msgs::YuvThresholdInit::Request& req, iris_msgs::YuvThresholdInit::Response& res)
{
    for (uint8_t i = 0; i < 6; i++)
        res.yuv_threshold.push_back(yuv_field_thresh[i]);
    return true;
}

bool CllbckSerYuvKeeperObs(iris_msgs::YuvThresholdInit::Request& req, iris_msgs::YuvThresholdInit::Response& res)
{
    for (uint8_t i = 0; i < 6; i++)
        res.yuv_threshold.push_back(yuv_keeper_obs[i]);
    return true;
}

bool CllbckSerYuvLine(iris_msgs::YuvThresholdInit::Request& req, iris_msgs::YuvThresholdInit::Response& res)
{
    for (uint8_t i = 0; i < 6; i++)
        res.yuv_threshold.push_back(yuv_line_thresh[i]);

    return true;
}

void SaveConfig(uint8_t var)
{
    try {
        char* robot_num = getenv("ROBOT");
        char config_file[100];
        std::string current_dir = ros::package::getPath("vision");
        sprintf(config_file, "%s/../../config/IRIS%s/dynamic_conf.yaml", current_dir.c_str(), robot_num);

        YAML::Node config = YAML::LoadFile(config_file);

        switch (var) {
        case 0:
            config["Vision"]["ball_y_min"] = to_string(yuv_ball_thresh[0]);
            config["Vision"]["ball_y_max"] = to_string(yuv_ball_thresh[1]);
            config["Vision"]["ball_u_min"] = to_string(yuv_ball_thresh[2]);
            config["Vision"]["ball_u_max"] = to_string(yuv_ball_thresh[3]);
            config["Vision"]["ball_v_min"] = to_string(yuv_ball_thresh[4]);
            config["Vision"]["ball_v_max"] = to_string(yuv_ball_thresh[5]);
            break;
        case 1:
            config["Vision"]["field_h_min"] = to_string(yuv_field_thresh[0]);
            config["Vision"]["field_h_max"] = to_string(yuv_field_thresh[1]);
            config["Vision"]["field_s_min"] = to_string(yuv_field_thresh[2]);
            config["Vision"]["field_s_max"] = to_string(yuv_field_thresh[3]);
            config["Vision"]["field_v_min"] = to_string(yuv_field_thresh[4]);
            config["Vision"]["field_v_max"] = to_string(yuv_field_thresh[5]);
            break;
        case 2:
            config["Vision"]["line_h_min"] = to_string(yuv_line_thresh[0]);
            config["Vision"]["line_h_max"] = to_string(yuv_line_thresh[1]);
            config["Vision"]["line_l_min"] = to_string(yuv_line_thresh[2]);
            config["Vision"]["line_l_max"] = to_string(yuv_line_thresh[3]);
            config["Vision"]["line_s_min"] = to_string(yuv_line_thresh[4]);
            config["Vision"]["line_s_max"] = to_string(yuv_line_thresh[5]);
            break;
        case 3:
            config["Vision"]["r_scan_min_cm"] = to_string(r_scan_min_cm);
            config["Vision"]["r_scan_max_cm"] = to_string(r_scan_max_cm);
            break;
        case 4:
            config["Vision"]["keeper_obs_y_min"] = to_string(yuv_keeper_obs[0]);
            config["Vision"]["keeper_obs_y_max"] = to_string(yuv_keeper_obs[1]);
            config["Vision"]["keeper_obs_u_min"] = to_string(yuv_keeper_obs[2]);
            config["Vision"]["keeper_obs_u_max"] = to_string(yuv_keeper_obs[3]);
            config["Vision"]["keeper_obs_v_min"] = to_string(yuv_keeper_obs[4]);
            config["Vision"]["keeper_obs_v_max"] = to_string(yuv_keeper_obs[5]);
            break;
        case 5:
            config["VisionSwitcher"]["field_switch"] = to_string(vision_swithcer_datas[0]);
            config["VisionSwitcher"]["ball_switch"] = to_string(vision_swithcer_datas[1]);
            config["VisionSwitcher"]["line_switch"] = to_string(vision_swithcer_datas[2]);
            break;
        case 6:
            config["PointPlanner"]["goal_k_thresh"] = to_string(k_passing[0]);
            config["PointPlanner"]["friend_k_thresh"] = to_string(k_passing[1]);
            config["PointPlanner"]["obs_rad_k_thresh"] = to_string(k_passing[2]);
            config["PointPlanner"]["obs_line_k_thresh"] = to_string(k_passing[3]);
            break;
        }

        std::ofstream fout(config_file);
        fout << config;
        fout.close();
    } catch (const std::exception& e) {
        ROS_ERROR("SAVE VISION CONFIG %s", e.what());
    }
}

void LoadConfig()
{
    char* robot_num = getenv("ROBOT");
    char config_file[100], color_cfg[100];
    std::string current_dir = ros::package::getPath("vision");
    sprintf(config_file, "%s/../../config/IRIS%s/static_conf.yaml", current_dir.c_str(), robot_num);
    sprintf(color_cfg, "%s/../../config/IRIS%s/dynamic_conf.yaml", current_dir.c_str(), robot_num);
    printf("config file: %s\n", config_file);

    YAML::Node config_static = YAML::LoadFile(config_file);
    g_cam_offset_x = config_static["Camera"]["cam_offset_x"].as<int>();
    g_cam_offset_y = config_static["Camera"]["cam_offset_y"].as<int>();

    YAML::Node color_config = YAML::LoadFile(color_cfg);
    yuv_ball_thresh[0] = color_config["Vision"]["ball_y_min"].as<int>();
    yuv_ball_thresh[1] = color_config["Vision"]["ball_y_max"].as<int>();
    yuv_ball_thresh[2] = color_config["Vision"]["ball_u_min"].as<int>();
    yuv_ball_thresh[3] = color_config["Vision"]["ball_u_max"].as<int>();
    yuv_ball_thresh[4] = color_config["Vision"]["ball_v_min"].as<int>();
    yuv_ball_thresh[5] = color_config["Vision"]["ball_v_max"].as<int>();

    yuv_field_thresh[0] = color_config["Vision"]["field_h_min"].as<int>();
    yuv_field_thresh[1] = color_config["Vision"]["field_h_max"].as<int>();
    yuv_field_thresh[2] = color_config["Vision"]["field_s_min"].as<int>();
    yuv_field_thresh[3] = color_config["Vision"]["field_s_max"].as<int>();
    yuv_field_thresh[4] = color_config["Vision"]["field_v_min"].as<int>();
    yuv_field_thresh[5] = color_config["Vision"]["field_v_max"].as<int>();

    yuv_line_thresh[0] = color_config["Vision"]["line_h_min"].as<int>();
    yuv_line_thresh[1] = color_config["Vision"]["line_h_max"].as<int>();
    yuv_line_thresh[2] = color_config["Vision"]["line_l_min"].as<int>();
    yuv_line_thresh[3] = color_config["Vision"]["line_l_max"].as<int>();
    yuv_line_thresh[4] = color_config["Vision"]["line_s_min"].as<int>();
    yuv_line_thresh[5] = color_config["Vision"]["line_s_max"].as<int>();

    yuv_keeper_obs[0] = color_config["Vision"]["keeper_obs_y_min"].as<int>();
    yuv_keeper_obs[1] = color_config["Vision"]["keeper_obs_y_max"].as<int>();
    yuv_keeper_obs[2] = color_config["Vision"]["keeper_obs_u_min"].as<int>();
    yuv_keeper_obs[3] = color_config["Vision"]["keeper_obs_u_max"].as<int>();
    yuv_keeper_obs[4] = color_config["Vision"]["keeper_obs_v_min"].as<int>();
    yuv_keeper_obs[5] = color_config["Vision"]["keeper_obs_v_max"].as<int>();

    vision_swithcer_datas[0] = color_config["VisionSwitcher"]["field_switch"].as<int>();
    vision_swithcer_datas[1] = color_config["VisionSwitcher"]["ball_switch"].as<int>();
    vision_swithcer_datas[2] = color_config["VisionSwitcher"]["line_switch"].as<int>();

    r_scan_min_cm = color_config["Vision"]["r_scan_min_cm"].as<int>();
    r_scan_max_cm = color_config["Vision"]["r_scan_max_cm"].as<int>();

    k_passing[0] = color_config["PointPlanner"]["goal_k_thresh"].as<int>();
    k_passing[1] = color_config["PointPlanner"]["friend_k_thresh"].as<int>();
    k_passing[2] = color_config["PointPlanner"]["obs_rad_k_thresh"].as<int>();
    k_passing[3] = color_config["PointPlanner"]["obs_line_k_thresh"].as<int>();
}
