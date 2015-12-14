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
extern ANDROID_TX_INFO_CHECKPOINTS_t *android_checkpoints_values;
extern ANDROID_TX_INFO_COORDINATES_t *android_coordinates_values;

/***************** CAN MESSAGE *****************/
can_msg_t *msg_tx;

/***************** CAN MESSAGE ID *****************/
msg_hdr_t android_stop_go_cmd_hdr = ANDROID_TX_STOP_GO_CMD_HDR;
msg_hdr_t android_info_checkpoint_hdr = ANDROID_TX_INFO_CHECKPOINTS_HDR;
msg_hdr_t android_info_coordinates_hdr = ANDROID_TX_INFO_COORDINATES_HDR;

/// This is the stack size used for each of the period tasks
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

/// Called once before the RTOS is started, this is a good place to initialize things once
bool period_init(void)
{
    uint32_t sglist[] = {ANDROID_TX_STOP_GO_CMD_HDR.mid,
                        ANDROID_TX_INFO_CHECKPOINTS_HDR.mid,
                        ANDROID_TX_INFO_COORDINATES_HDR.mid};
    size_t sizeOfArray = (sizeof(sglist) / sizeof(*sglist));
    iCAN_init_FULLCAN(sglist, sizeOfArray);

    msg_tx = new can_msg_t{0};
    android_stop_go_values = new ANDROID_TX_STOP_GO_CMD_t {0};
    android_checkpoints_values = new ANDROID_TX_INFO_CHECKPOINTS_t {0};
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
    if (android_stop_go_values->ANDROID_STOP_CMD_signal == 0)
    {
      /*  printf("%d %d %f %f\n", android_stop_go_values->ANDROID_STOP_CMD_signal,
                                    android_checkpoints_values->ANDROID_INFO_CHECKPOINTS_count,
                                    android_coordinates_values[1].GPS_INFO_COORDINATES_lat,
                                    android_coordinates_values[1].GPS_INFO_COORDINATES_long);
*/
        msg_tx->msg_id = (uint32_t)ANDROID_TX_STOP_GO_CMD_HDR.mid;
        msg_hdr_t encoded_message = ANDROID_TX_STOP_GO_CMD_encode(&(msg_tx->data.qword), android_stop_go_values);

        if (iCAN_tx(msg_tx, &encoded_message))
        {
           printf("STOP message sent!\n");
        }
    }
    else if (android_stop_go_values->ANDROID_STOP_CMD_signal == 1)
    {
        msg_tx->msg_id = (uint32_t)ANDROID_TX_STOP_GO_CMD_HDR.mid;
        msg_hdr_t encoded_message = ANDROID_TX_STOP_GO_CMD_encode((uint64_t*)&(msg_tx->data.qword), android_stop_go_values);

        if (iCAN_tx(msg_tx, &encoded_message))
        {
            printf("GO message sent to master\n");
        }

        msg_tx->msg_id = (uint32_t)ANDROID_TX_INFO_CHECKPOINTS_HDR.mid;
        encoded_message = ANDROID_TX_INFO_CHECKPOINTS_encode((uint64_t*)&(msg_tx->data.qword), android_checkpoints_values);
        if (iCAN_tx(msg_tx, &encoded_message))
        {
            printf("checkpoint sent!\n");
        }
    }
}

void period_100Hz(void)
{
    LE.toggle(3);


  /*  if ( count == android_checkpoints_values->ANDROID_INFO_CHECKPOINTS_count)
    {
        // printf("All coordinates are sent.\n");
    }
    else
    {*/
       /* msg_tx->msg_id = (uint32_t)ANDROID_TX_INFO_COORDINATES_HDR.mid;
        msg_hdr_t message = ANDROID_TX_INFO_COORDINATES_encode((uint64_t*)&(msg_tx->data.qword), &android_coordinates_values[count++]);
        if (iCAN_tx(msg_tx, &message))
        {
            printf("android_coordinates_values sent");
        }*/

}

void period_1000Hz(void)
{
    LE.toggle(4);
}
