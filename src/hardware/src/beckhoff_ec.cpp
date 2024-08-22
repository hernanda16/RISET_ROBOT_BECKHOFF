#include "hardware/beckhoff_ec.hpp"

Beckhoff_EC* ec = Beckhoff_EC::getInstance();

int main(int argc, char** argv)
{
    ros::init(argc, argv, "beckhoff_ec");
    ros::NodeHandle nh;
    ros::MultiThreadedSpinner spinner(0);

    printf("Beckhoff EC\n");

    ec->init(&nh);

    spinner.spin();
    return 0;
}

void Beckhoff_EC::callbackRoutine(const ros::TimerEvent& event)
{
    ec_send_processdata();
    wkc = ec_receive_processdata(EC_TIMEOUTRET);

    if (wkc >= expectedWKC) {
        printf("Button %d: %d\n", 1, (in_EL1809_->data) & 0x01);
        printf("ADC    %d: %f\n", 1, ((float)in_EL3104_->value1 / 32767.0) * 10.0);
        printf("RTD    %d: %f\n", 1, (float)in_EL3204_->value1 * 0.1);
    }
}

void Beckhoff_EC::init(ros::NodeHandle* nh)
{
    if (ec_init("enx14ebb61961e6")) {
        printf("EtherCAT initialized.\n");
        if (ec_config_init(FALSE) > 0) {
            printf("%d slaves found and configured.\n", ec_slavecount);

            ec_config_map(&IOmap);
            ec_configdc();

            uint16_t current_state;
            checkAndSet(0, EC_STATE_SAFE_OP, &current_state);
            if (current_state != EC_STATE_SAFE_OP) {

                // Set slave ke State Init
                if (current_state == EC_STATE_SAFE_OP) {
                    checkAndSet(0, EC_STATE_INIT, &current_state);
                } else {
                    printf("Failed to set to Safe Operational State\n");
                }

                // Set ke Pre Operational State
                if (current_state == EC_STATE_INIT) {
                    checkAndSet(0, EC_STATE_PRE_OP, &current_state);
                } else {
                    printf("Failed to set to Init State\n");
                }

                // Set ke Safe Operational State
                if (current_state == EC_STATE_PRE_OP) {
                    checkAndSet(0, EC_STATE_SAFE_OP, &current_state);
                } else {
                    printf("Failed to set to Pre-Operational State\n");
                }
            }

            if (current_state == EC_STATE_SAFE_OP) {

                oloop = ec_slave[0].Obytes;
                printf("Obytes: %d\n", oloop);
                if ((oloop == 0) && (ec_slave[0].Obits > 0))
                    oloop = 1;
                if (oloop > 8)
                    oloop = 8;

                iloop = ec_slave[0].Ibytes;
                printf("Ibytes: %d\n", iloop);
                if ((iloop == 0) && (ec_slave[0].Ibits > 0))
                    iloop = 1;
                if (iloop > 8)
                    iloop = 8;

                printf("segments : %d : %d %d %d %d\n", ec_group[0].nsegments, ec_group[0].IOsegment[0], ec_group[0].IOsegment[1], ec_group[0].IOsegment[2], ec_group[0].IOsegment[3]);
                expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;
                printf("%d %d -> %d\n", ec_group[0].outputsWKC, ec_group[0].inputsWKC, expectedWKC);

                checkAndSet(0, EC_STATE_OPERATIONAL, &current_state);
            } else {
                printf("Failed to set to Safe Operational State\n");
            }

            if (ec_slave[0].state == EC_STATE_OPERATIONAL) {
                printf("Operational state reached for all slaves.\n");

                ec_send_processdata();
                ec_receive_processdata(EC_TIMEOUTRET);

                for (uint8_t slave = 1; slave <= ec_slavecount; slave++) {
                    switch (ec_slave[slave].eep_id) {
                    case EL1809_ID:
                        printf("EL1809 Found\n");
                        in_EL1809_ = (in_EL1809t*)ec_slave[slave].inputs;
                        break;
                    case EL3104_ID:
                        printf("EL3104 Found\n");
                        in_EL3104_ = (in_EL3104t*)ec_slave[slave].inputs;
                        break;
                    case EL3204_ID:
                        printf("EL3204 Found\n");
                        in_EL3204_ = (in_EL3204t*)ec_slave[slave].inputs;
                        break;
                    default:
                        printf("Unknown Slave Found\n");
                        break;
                    }
                }
            } else {
                printf("Not all slaves reached operational state.\n");
                ec_readstate();
                for (int slave = 1; slave <= ec_slavecount; slave++) {
                    if (ec_slave[slave].state != EC_STATE_OPERATIONAL) {
                        printf("Slave %d State: %d\n", slave, ec_slave[slave].state);
                    }
                }
            }
        }
    } else {
        printf("EtherCAT initialization failed.\n");
    }

    this->tim_routine = nh->createTimer(ros::Duration(0.01), std::bind(&Beckhoff_EC::callbackRoutine, this, std::placeholders::_1));
}

void Beckhoff_EC::checkAndSet(int slave, uint16_t target_state, uint16_t* current_state)
{
    *current_state = ec_statecheck(slave, target_state, EC_TIMEOUTSTATE);
    printf("State Check: %d => %d\n", *current_state, target_state);

    if (*current_state != target_state) {
        ec_slave[slave].state = target_state;
        ec_writestate(slave);
        *current_state = ec_statecheck(slave, target_state, EC_TIMEOUTSTATE);
        printf("State Check (after write): %d => %d\n", *current_state, target_state);
    }
}