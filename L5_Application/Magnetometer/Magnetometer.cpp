/*
 * Magnetometer.cpp
 *
 *  Created on: Apr 27, 2015
 *      Author: calvin.lai
 */

#include <stdio.h>

#include "tasks.hpp"
#include "io.hpp"
#include "c_tlm_var.h"
#include "io.hpp"
#include "Magnetometer.hpp"
#include "can.h"

#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)



Magnetometer::Magnetometer(uint8_t priority) : scheduler_task("Magnetometer", 2058, priority){
     setRunDuration(1000); // can be changed
}

bool Magnetometer::run(void *p){
    float heading;
    heading = MS.getHeading();
    printf("Heading: %f\n", heading);
    bool sent = 0;
    can_msg_t can_msg_1 = {0};
    can_msg_1.msg_id = 0x201;
    can_msg_1.frame_fields.data_len = 0x0001;
    can_msg_1.data.dwords[0] = heading;
    can_msg_1.frame_fields.is_rtr = 0;
    can_msg_1.frame_fields.is_29bit = 0;
    if(sent){
        can_msg_1.data.dwords[0] = 0;
        sent = 0;
    }
    if(CAN_tx(can1, &can_msg_1, 100)){
        printf("CAN msg: %f sent out through can bus 1\n", can_msg_1.data.dwords[0]);
        printf("TX watermark: %i\n", CAN_get_tx_watermark(can1));
        printf("TX sent: %i\n", CAN_get_tx_count(can1));
        sent = 1;
    }
    return true;
}




