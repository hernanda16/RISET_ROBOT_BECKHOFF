/**
 * @author Hernanda Achmad Priyatna - hernanda16
 * @brief Handling communication with Beckhoff EtherCAT
 */

#ifndef BECKHOFF_EC_HPP
#define BECKHOFF_EC_HPP

//--ROS Headers
#include <ros/package.h>
#include <ros/ros.h>

//--ROS Messages

//--C++ Headers
#include <iostream>
#include <string.h>
#include <unistd.h>

//--Beckhoff Headers
#include <soem/ethercat.h>

#define EC_TIMEOUTMON 5000

#define EL1809_ID 0x07113052
#define EL3104_ID 0x0c203052
#define EL3204_ID 0x0c843052

typedef struct PACKED {
    uint16_t data;
} digital_io_t;

typedef struct PACKED {
    uint16 status1;
    int16 value1;
    uint16 status2;
    int16 value2;
    uint16 status3;
    int16 value3;
    uint16 status4;
    int16 value4;
} analog_io4_t;

typedef digital_io_t in_EL1809t;
typedef analog_io4_t in_EL3104t;
typedef analog_io4_t in_EL3204t;

class Beckhoff_EC {
private:
    Beckhoff_EC() {};
    ~Beckhoff_EC() {};

    uint8 IOmap[4096]; // I/O map for PDOs
    volatile int wkc; // working counter
    int expectedWKC; // expected working counter
    int oloop, iloop; // loop counters

    in_EL1809t* in_EL1809_;
    in_EL3104t* in_EL3104_;
    in_EL3204t* in_EL3204_;

public:
    static Beckhoff_EC*
    getInstance()
    {
        static Beckhoff_EC instance_;
        return &instance_;
    }

    Beckhoff_EC(Beckhoff_EC const&) = delete;
    void operator=(Beckhoff_EC const&) = delete;

    //--Prototypes
    void init(ros::NodeHandle* nh);
    void checkAndSet(int slave, uint16_t target_state, uint16_t* current_state);

    void callbackRoutine(const ros::TimerEvent& event);

    //--Subscribers

    //--Publishers

    //--Timer
    ros::Timer tim_routine;
};

#endif