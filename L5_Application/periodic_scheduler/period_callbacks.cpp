/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

/**
 * @file
 * This contains the period callback functions for the periodic scheduler
 *
 * @warning
 * These callbacks should be used for hard real-time system, and the priority of these
 * tasks are above everything else in the system (above the PRIORITY_CRITICAL).
 * The period functions SHOULD NEVER block and SHOULD NEVER run over their time slot.
 * For example, the 1000Hz take slot runs periodically every 1ms, and whatever you
 * do must be completed within 1ms.  Running over the time slot will reset the system.
 */

#include <gps_car.hpp>
#include <stdio.h>
#include <stdint.h>
#include "io.hpp"
#include "periodic_callback.h"
#include "file_logger.h"
#include "243_can/CAN_structs.h"
#include "243_can/iCAN.hpp"


/// This is the stack size used for each of the period tasks
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

void period_1Hz(void)
{
    LE.toggle(1);
    can_msg_t msg = {0};
    msg.msg_id = 0x200;
    msg.frame = 0;
    msg.frame_fields.is_29bit = 0;
    msg.frame_fields.data_len = 8;
    msg.data.bytes[0] = 1;
//    can_fullcan_msg_t *fc1 = CAN_fullcan_get_entry_ptr(CAN_gen_sid(can2, 0x200));
//    gps_car_send_heading(); /// XXX: This should probably be 10 or 100Hz
//    gps_car_send_heartbeat();
    if(CAN_is_bus_off(can1)){
        printf("Can1 is off\n");
        CAN_reset_bus(can1);
    }
    if(CAN_is_bus_off(can2)){
        printf("Can2 is off\n");
        CAN_reset_bus(can2);
    }
    printf("Sending message through can1\n");
    if(CAN_tx(can1, &msg, 10)){
        printf("Successfully sent message through can 1\n\n\n");
    }
    else{
        printf("Could not send through can 1\n");
    }
    can_msg_t msg1 = {0};
    msg1.msg_id = 0x100;
    msg1.frame = 0;
    msg1.frame_fields.is_29bit = 0;
    msg1.frame_fields.data_len = 8;
    msg1.data.bytes[0] = 1;
    can_fullcan_msg_t fc;
//    if(CAN_tx(can2, &msg1, 10)) iCAN_rx(can1, 0x100);
    if(iCAN_rx(&fc, 0x200)){
        printf("Received the following from can2:\n");
        printf("msg_id: %x\n", fc.msg_id);
        printf("data: %i\n\n\n", fc.data.bytes[0]);
        msg.msg_id = 0x204;
        msg.data.bytes[0] = 2;
        CAN_tx(can1, &msg, 10);
        iCAN_rx(&fc, 0x204);
        printf("Received the following from can2:\n");
        printf("msg_id: %x\n", fc.msg_id);
        printf("data: %i\n\n\n", fc.data.bytes[0]);

    }
}

void period_10Hz(void)
{
    LE.toggle(2);
}

void period_100Hz(void)
{
    LE.toggle(3);
}

void period_1000Hz(void)
{
    LE.toggle(4);
}
