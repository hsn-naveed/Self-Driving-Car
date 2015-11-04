/*
 * full_can_api.cpp
 *
 *  Created on: Nov 2, 2015
 *      Author: Hassan
 */

#include "full_can_api.h"
#include "io.hpp"

full_can_api :: full_can_api(){
    bus_init();
}

bool full_can_api :: bus_init(){
    can_t bus = can1;
    uint32_t baudrate_kbps = 100;
    uint16_t rxq_size = 50;
    uint16_t txq_size = 50;
    const uint16_t id_1 = 0x704;         //Master receiving from Motor
    const uint16_t id_2 = 0x14A;         //Master receiving from Sensor
    const uint16_t id_3 = 0x215;         //Master commands motor
    const uint16_t id_4 = 0x04A;         //Master receiving from Android

    if (CAN_init(bus, baudrate_kbps, rxq_size, txq_size,NULL,NULL)){
        if(1)
            CAN_bypass_filter_accept_all_msgs();
        //        if(CAN_fullcan_add_entry(bus, CAN_gen_sid(bus, id_1), CAN_gen_sid(bus, id_2))){
//            const can_std_id_t slist[]      = {
//                    CAN_gen_sid(can1, id_4), CAN_gen_sid(can1, id_2),   // 2 entries
//                    CAN_gen_sid(can1, id_3), CAN_gen_sid(can1, id_1)      // Ascending Order
//            };
//            const can_std_grp_id_t sglist[] = { {CAN_gen_sid(can1, 0x01), CAN_gen_sid(can1, 0x200)}, // Group 1
//                    {CAN_gen_sid(can2, 0x201), CAN_gen_sid(can2, 0x500)}  // Group 2
//            };
//            const can_ext_id_t *elist = NULL;
//            const can_ext_grp_id_t *eglist= NULL;
//            if(CAN_setup_filter(slist, 4, sglist, 2, elist, 0, eglist, 0)){
//                puts("\nIn Full Can Mode and Filter Setup!");
//            }
//
//        }
        else{
            puts("\nFULL CAN FAILED");
        }
        CAN_reset_bus(bus);
        puts("\nInitialization Successful\n");
        return true;
    }
    else{
        puts("\nFailed to Initialize");
        return false;
    }
}

bool full_can_api ::can_rec(uint16_t mail_id){
    can_fullcan_msg_t* msgPtr;
    if(1){
        can_t bus = can1;

        //uint32_t timeout_ms = portMAX_DELAY;
        can_fullcan_msg_t msg = { 0 };
        msgPtr = CAN_fullcan_get_entry_ptr(CAN_gen_sid(bus, mail_id));

        can_msg_t msg_tx = { 0 };

        if(CAN_fullcan_read_msg_copy(msgPtr,&msg)){
            puts("\nReceived!");
            //pass the received message to the period_callback functions

            return true;
        }


        else{
            puts ("\nNo message received!");
            return false;
        }

    }
    else{
        return false;
    }
}

bool full_can_api ::can_send(can_msg_t *msg, uint32_t timeout_ms){
    can_t bus = can1;

    if(CAN_tx (bus, msg, timeout_ms)){
        puts("\nMessage Sent!");
       // LE.on(1);
        //vTaskDelay(0);
        //LE.off(1);
        return true;
    }

    else{
        puts ("\nNo message could be sent !");
        return true;
    }

}

/*
bool full_can_api ::run(void *p){
    //nothing to run
    return true;
}
*/
