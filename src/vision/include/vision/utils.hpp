#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstring>
#include <fstream>
#include <ros/package.h>
#include <stdint.h>

void loadLUT(int16_t* lut_src2dst, bool is_from_frame = true)
{
    char LUT_file[100];
    char* robot_num;
    robot_num = getenv("ROBOT");
    std::string current_dir = ros::package::getPath("world_model");

    if (is_from_frame)
        sprintf(LUT_file, "%s/../../config/IRIS%s/lut_px2cm_msi_kri2.bin", current_dir.c_str(), robot_num);
    // sprintf(LUT_file, "%s/../../config/IRIS%s/kri/azzam_lut1000.bin", current_dir.c_str(), robot_num);
    // sprintf(LUT_file, "%s/../../config/IRIS%s/kri/kri_msi_lut500.bin.bin", current_dir.c_str(), robot_num);

    else
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

#endif