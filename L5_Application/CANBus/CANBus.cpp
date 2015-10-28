/*
 * CANBus.cpp
 */

#include "CANBus.hpp"

#if 1
#define MOTOR_CAN_ID 0x215
#define LCD_CAN_ID 0x295
#endif


CANBus::CANBus(uint8_t priorityToUse) : scheduler_task("canBusTask", 2048, priorityToUse){
    canBusObj = can1;
}

bool CANBus::init(void){
    if (CAN_init(canBusObj, canBusBaudRate_Kbps, canBusRxQueueSize, canBusTxQueueSize,
            NULL, NULL)){
        if (CAN_fullcan_add_entry(canBusObj, CAN_gen_sid(canBusObj, 0x215), CAN_gen_sid(canBusObj, 0x295))){
            const can_std_id_t sList[] = {
                   CAN_gen_sid(canBusObj, MOTOR_CAN_ID), CAN_gen_sid(canBusObj, LCD_CAN_ID)};

            const can_std_grp_id_t sGList[] = {
                    // Arbitrary initialization of can1, and can2 entries for hardware
                   {CAN_gen_sid(canBusObj, 0x01), CAN_gen_sid(canBusObj, 0x199)},
                   {CAN_gen_sid(can2, 0x200), CAN_gen_sid(can2, 0x500)}};

            const can_ext_id_t *elist       = NULL; // Not used, so set it to NULL
            const can_ext_grp_id_t *eglist = NULL; // Not used, so set it to NULL

            if (CAN_setup_filter(sList, 2, sGList, 2, elist, 0, eglist, 0)){
             puts("\nSuccessfully setup filter, and now in FULL CAN mode\n");
            }
            else{
                puts("\nCAN_setup_filter failed!\n");
            }
        }
        else{
            puts("\nFULL CAN SETUP FAILED");
        }

        CAN_reset_bus(canBusObj);
        puts("\nInitialiazed can1 bus\n");
        return true;
    }
    else{
        puts("\nFailed to initialize Full can mode\n");
        return false;
    }
}

bool CANBus::run(void *p){
    // Motor full can message/pointer
    can_fullcan_msg_t motorFullCanMessage;
    can_fullcan_msg_t *motorfullCanMessagePointer = CAN_fullcan_get_entry_ptr(CAN_gen_sid(canBusObj, MOTOR_CAN_ID));

    //LCD full can message/pointer
    can_fullcan_msg_t lcdFullCanMessage;
    can_fullcan_msg_t *lcdFullCanMessagePointer = CAN_fullcan_get_entry_ptr(CAN_gen_sid(canBusObj, LCD_CAN_ID));

    if (CAN_fullcan_read_msg_copy(motorfullCanMessagePointer, &motorFullCanMessage)){

    }
}

