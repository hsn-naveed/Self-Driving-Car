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
#include "shared_handles.h"
#include "tasks.hpp"
#include <inttypes.h>

#include "master_control.hpp"
#include "can.h"

#include <L4_IO/can_definitions.hpp>
#include "L4_IO/can_storage.hpp"




/// This is the stack size used for each of the period tasks
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);
//int timer_count = 0; //to avoid keep sending zeros

uint8_t current_mode = 0;

scheduler_task *myTask = scheduler_task::getTaskPtrByName("control_handler_task");

//current state
volatile int current_state_free_run = 0;

can_msg_t msg_rx = { 0 };
can_msg_t msg_tx = { 0 };




void period_1Hz(void)
{
     LE.toggle(1);
     printf ("CAN_STORAGE MOTOR VALUE %d\n", CAN_ST.getMotorSpeed());
    //sen_value = scheduler_task::getSharedObject(shared_sensor_data);

     uint8_t *sampleArray;
     sampleArray = CAN_ST.getSensorValues();
     for (int i = 0; i < SIZE_OF_SENSOR_ARRAY; i++) {
         printf("SENSOR [%d]: %d\n", i, sampleArray[i]);
     }

}

void period_10Hz(void)
{
   // LE.toggle(2);




        //message coming from the terminal
        if(xQueueReceive(scheduler_task::getSharedObject(shared_CAN_message_queue_terminal), &msg_rx, 0) ||
                //message coming from the real CAN receiver
                xQueueReceive(scheduler_task::getSharedObject(shared_CAN_message_queue_receive), &msg_rx, 0) )    {
            //send to control_handler_task
            xQueueSend(scheduler_task::getSharedObject(shared_CAN_message_queue_master), &msg_rx, 0);
        }

        //Handle if no message is received
        else    {
            msg_rx.msg_id = 0xFFF; //this one should be removed
            xQueueSend(scheduler_task::getSharedObject(shared_CAN_message_queue_master), &msg_rx, 0);
        }

        if (current_mode == FREERUN_MODE)   {
            //state machine for free run
            switch(current_state_free_run)  {

                //request sensor readings
                case 0:
                    //put our message id
                    msg_tx.msg_id = MASTER_COMMANDS_SENSOR;
                    //pass our message to the master_control tasks
                    //to handle the data parameters
                    xQueueSend(scheduler_task::getSharedObject(shared_CAN_message_queue_master), &msg_tx, 0);

                    printf("current_state_free_run %d finished\n", current_state_free_run);
                    //move to the next state
                    current_state_free_run = 1;

                    break;
                //read sensor readings
                //during this state, the sensor readings should've been received (from sensor team)
                //and our master_control task should've been updated the new sensor values
                //Send Motor commands in this state as well
                case 1:
                    //Motor

                  /* if( ((uint8_t)0x00 == global_sensor_value[(int)LEFT_SENSOR]) &&
                           ((uint8_t)0x00 == global_sensor_value[(int)MIDDLE_SENSOR]) &&
                           ((uint8_t)0x00 == global_sensor_value[(int)RIGHT_SENSOR]))  {

                   }*/
                    printf("current_state_free_run %d finished\n", current_state_free_run);
                    current_state_free_run = 2;
                    break;

                case 2:
                    printf("current_state_free_run %d finished\n", current_state_free_run);
                    //reset state
                    current_state_free_run = 0;
                    break;

                default:
                    break;

            }
        }

        //send the whole message to the control_handler_task
        //xQueueSend(scheduler_task::getSharedObject(shared_CAN_message_queue_master), &msg_rx, 0);


//        if (xQueueReceive(mCAN_QueueHandler, &msg_rx, 0)) {
//
//            xQueueSend(scheduler_task::getSharedObject(shared_CAN_message_queue_master), &msg_rx, 0);
//            printf("Sent message:\n");
//        }


       //int msg = SW.getSwitchValues();
/*
       if(msg == 0) timer_count++;

       if(timer_count < 4 && (0 == msg)) {
           xQueueSend(scheduler_task::getSharedObject(shared_LEDSignalForCAN), &msg, portMAX_DELAY);
       }
       else if (msg != 0)  {
           xQueueSend(scheduler_task::getSharedObject(shared_LEDSignalForCAN), &msg, portMAX_DELAY);
           timer_count = 0;
       }

       xSemaphoreGive(scheduler_task::getSharedObject(shared_CAN_Semaphore_Rx));*/

}

void period_100Hz(void)
{
   // LE.toggle(3);
    int sw_value = SW.getSwitchValues();
    if (sw_value == 1){
        LD.clear();
        LD.setLeftDigit('L');
        LD.setRightDigit('1');
        current_mode = MANUAL_MODE;
        printf("MANUAL MODE SELECTED\n");
    }
    else if (sw_value == 2) {
        LD.clear();
        LD.setLeftDigit('A');
        LD.setRightDigit('2');
        current_mode = AUTO_MODE;
        printf("AUTO MODE SELECTED\n");
    }
    else if (sw_value == 4){
        LD.clear();
        LD.setLeftDigit('F');
        LD.setRightDigit('3');
        current_mode = FREERUN_MODE;
        printf("FREERUN MODE SELECTED\n");
    }

}

void period_1000Hz(void)
{
    //LE.toggle(4);
}


void handleMessage()    {


}


