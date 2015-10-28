/*
 * CANBus.cpp
 */

#include "CANBus.hpp"

CANBus::CANBus(uint8_t priorityToUse) : scheduler_task("canBusTask", 2048, priorityToUse){
    canBusObj = can1;
}

//bool CANBus::init(void){
//    if (CAN_init(canBusObj, canBusBaudRate_Kbps, canBusRxQueueSize, canBusTxQueueSize,
//            NULL, NULL)){
//        if (CAN_fullcan_add_entry(canBusObj, CAN_gen_sid(canBusObj, 0x8), CAN_gen_sid(canBusObj, 0x9))){
//            const can_std_id_t sList[] = {
//                   CAN_gen_sid(canBusObj, 0x8), CAN_gen_sid(canBusObj, 0x9),
//                    CAN_get_sid(canBusObj, 0xA), CAN_get_sid(canBusObj, 0xB)};
//            };
//            const can_std_grp_id_t sgList[] = { CAN_gen_sid(canBusObj, )
//        }
//    }
//}

