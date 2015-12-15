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

#include <stdio.h>
#include <stdint.h>
#include "io.hpp"
#include "periodic_callback.h"
#include "file_logger.h"
#include "243_can/iCAN.hpp"
#include "243_can/CAN_structs.h"

/***************** ANDROID *****************/
extern ANDROID_TX_STOP_GO_CMD_t *android_stop_go_values;
extern ANDROID_TX_INFO_CHECKPOINTS_t *android_checkpoints_count;
extern ANDROID_TX_INFO_COORDINATES_t *android_coordinates_values;
extern bool g_flagTransmitToCAN;
extern bool g_flagTransmitCheckpointCount;

/***************** CAN MESSAGE *****************/
can_msg_t *msg_tx;

/***************** CAN MESSAGE ID *****************/
msg_hdr_t android_stop_go_cmd_hdr = ANDROID_TX_STOP_GO_CMD_HDR;
msg_hdr_t android_info_checkpoint_hdr = ANDROID_TX_INFO_CHECKPOINTS_HDR;
msg_hdr_t android_info_coordinates_hdr = ANDROID_TX_INFO_COORDINATES_HDR;

/***************** PERIODIC COUNTER *****************/
uint8_t g_checkpoints_counter = 0;

/// This is the stack size used for each of the period tasks
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

/// Called once before the RTOS is started, this is a good place to initialize things once
bool period_init(void)
{
    uint32_t sglist[] = {GPS_TX_DESTINATION_REACHED_HDR.mid};
    size_t sizeOfArray = (sizeof(sglist) / sizeof(*sglist));
    iCAN_init_FULLCAN(sglist, sizeOfArray);

    msg_tx = new can_msg_t{0};
    android_stop_go_values = new ANDROID_TX_STOP_GO_CMD_t {0};
    android_checkpoints_count = new ANDROID_TX_INFO_CHECKPOINTS_t {0};
    android_coordinates_values = new ANDROID_TX_INFO_COORDINATES_t {0};
    android_stop_go_values->ANDROID_STOP_CMD_signal = (uint8_t)-1;

    return true; // Must return true upon success
}

/// Register any telemetry variables
bool period_reg_tlm(void)
{
    // Make sure "SYS_CFG_ENABLE_TLM" is enabled at sys_config.h to use Telemetry
    return true; // Must return true upon success
}

void period_1Hz(void)
{
    LE.toggle(1);
}

void period_10Hz(void)
{
    /* STOP */
    if (android_stop_go_values->ANDROID_STOP_CMD_signal == 0)
    {
        /* Send stop */
        msg_hdr_t encoded_message = ANDROID_TX_STOP_GO_CMD_encode((uint64_t*)&(msg_tx->data.qword), android_stop_go_values);
        encoded_message.mid = (uint32_t)ANDROID_TX_STOP_GO_CMD_HDR.mid;

        if (iCAN_tx(msg_tx, &encoded_message))
        {
           printf("STOP message sent!\n");
        }
    }

    /* GO */
    else if (android_stop_go_values->ANDROID_STOP_CMD_signal == 1)
    {
        /* Send go */
        msg_hdr_t encoded_message = ANDROID_TX_STOP_GO_CMD_encode((uint64_t*)&(msg_tx->data.qword), android_stop_go_values);
        encoded_message.mid = (uint32_t)ANDROID_TX_STOP_GO_CMD_HDR.mid;


        if (iCAN_tx(msg_tx, &encoded_message))
        {
            printf("GO message sent to master\n");
        }
    }
}

void period_100Hz(void)
{
    LE.toggle(3);

    if (g_flagTransmitToCAN)    //Flag to transmit to CAN
    {
        msg_hdr_t message;
        /*
         * Counter will increment till it is at the total of checkpoints.
         * IE: (i = 0; i < totalNumberOfCheckpoints; i++);
         */
        if (g_checkpoints_counter == (android_checkpoints_count->ANDROID_INFO_CHECKPOINTS_count - 1))
            g_flagTransmitToCAN = false;


        if (g_flagTransmitCheckpointCount)
        {
            /* Send number of checkpoints */
            message = ANDROID_TX_INFO_CHECKPOINTS_encode((uint64_t*)&(msg_tx->data.qword), android_checkpoints_count);
            message.mid = (uint32_t)ANDROID_TX_INFO_CHECKPOINTS_HDR.mid;

            if (iCAN_tx(msg_tx, &message))
            {
                printf("checkpoint sent!\n");
            }
        }

        /* Send the coordinates incrementally */
        message = ANDROID_TX_INFO_COORDINATES_encode((uint64_t*)&(msg_tx->data.qword),
                        &android_coordinates_values[g_checkpoints_counter++]);
        message.mid = (uint32_t)ANDROID_TX_INFO_COORDINATES_HDR.mid;

        if (iCAN_tx(msg_tx, &message))
        {
            printf("android_coordinates_values sent");
        }
    }
}

void period_1000Hz(void)
{
    LE.toggle(4);
}
