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


int current_mode = 0;

/// This is the stack size used for each of the period tasks
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

can_msg_t msg_rx = { 0 };
can_msg_t msg_tx = { 0 };


/// Called once before the RTOS is started, this is a good place to initialize things once
bool period_init(void)
{
    return true; // Must return true upon success
}
uint8_t reverse_counter = 0;

can_msg_t msg_rx = { 0 };
can_msg_t msg_tx = { 0 };


//currently not used
can_data_t periodic_sensor_data = { 0 };

/// Register any telemetry variables
bool period_reg_tlm(void)
{
    // Make sure "SYS_CFG_ENABLE_TLM" is enabled at sys_config.h to use Telemetry
    return true; // Must return true upon success
}



void period_1Hz(void)
{


     LE.toggle(1);
    // printf ("CAN_STORAGE MOTOR VALUE %d\n", CAN_ST.getMotorSpeed());
    //sen_value = scheduler_task::getSharedObject(shared_sensor_data);

}

void period_10Hz(void)
{

    //reset freerun state counter
   // current_state_free_run = 0;

   // LE.toggle(2);


    CAN_ST.motor_data = (mast_mot_msg_t*) & msg_tx.data.bytes[0];
   // CAN_ST.sensor_data = (sen_msg_t*) & periodic_sensor_data.bytes[0];
       // if (current_mode == FREERUN_MODE)   {
            //state machine for free run
           // while( current_state_free_run != 12) {
                switch(current_state_free_run)  {

                    printf("switch entered\n");
                    //check sensor values
                    case 0:
                        prev_state_free_run = current_state_free_run;
                        //check if we are reversing
                        if (reverse_counter > 0)   {
                            reverse_counter--;
                            //go to 4 to handle reverse
                            current_state_free_run = 4;
                            printf("reverse count: %d\n", reverse_counter);
                            printf("state %d -> %d\n", prev_state_free_run, current_state_free_run);
                            break; //we break here because we want to go straight to state 4
                        }

                        if (CAN_ST.sensor_data->L >= (int) MINIMUM_SENSOR_VALUE &&
                                CAN_ST.sensor_data->M >= (int) MINIMUM_SENSOR_BLOCKED_VALUE &&
                                CAN_ST.sensor_data->R >= (int) MINIMUM_SENSOR_VALUE)    {


                            //move to the next state to follow GPS data
                            current_state_free_run = 1;
                            printf("state %d -> %d\n", prev_state_free_run, current_state_free_run);
                        } else  {
                            //command motor based on sensor values
                            current_state_free_run = 2;
                            printf("state %d -> %d\n", prev_state_free_run, current_state_free_run);
                        }


                       // printf("SENSOR VALUES IN PERIODIC: %d %d %d %d", periodic_sensor_data.bytes[3], periodic_sensor_data.bytes[2], periodic_sensor_data.bytes[1], periodic_sensor_data.bytes[0] );
                        break;

                    // state where GPS and heading values are respected
                    case 1:
                        prev_state_free_run = current_state_free_run;
                        //TO DO:
                        //GPS IMPLEMENTATION
                        //1. check heading

                        //RIGHT NOW WE JUST MOVE IT FORWARD
                        CAN_ST.motor_data->FRS = (uint8_t) COMMAND_MOTOR_FORWARD;
                        CAN_ST.motor_data->LR = (uint8_t) COMMAND_MOTOR_STRAIGHT; //turn left
                        CAN_ST.motor_data->SPD = (uint8_t) COMMAND_MOTOR_MEDIUM;
                        current_state_free_run = 3;
                        printf("state %d -> %d\n", prev_state_free_run, current_state_free_run);


                        break;

                    //Command motor based on sensor values
                    case 2:
                        prev_state_free_run = current_state_free_run;
                        //if middle is blocked
                       if ( CAN_ST.sensor_data->M < (int) MINIMUM_SENSOR_BLOCKED_VALUE )    {

                           //Command motor to stop
                           CAN_ST.motor_data->FRS = (uint8_t) COMMAND_MOTOR_STOP;
                           CAN_ST.motor_data->LR = (uint8_t) COMMAND_MOTOR_STRAIGHT; //turn straight
                           CAN_ST.motor_data->SPD = (uint8_t) COMMAND_MOTOR_SLOW; //no use, because we told it to STOP

                           msg_tx.msg_id = (uint32_t) MASTER_COMMANDS_MOTOR;
                           xQueueSend(scheduler_task::getSharedObject(shared_CAN_message_queue_master), &msg_tx, 0);

                           //here we set the reverse_count so we reverse n-times tick
                           reverse_counter = 4;

                           //then Handle back up
                           current_state_free_run = 4;
                           printf("state %d -> %d\n", prev_state_free_run, current_state_free_run);
                       }
                        //if only left sensor is blocked
                       else if (CAN_ST.sensor_data->L < (int) MINIMUM_SENSOR_VALUE &&
                            CAN_ST.sensor_data->M >= (int) MINIMUM_SENSOR_BLOCKED_VALUE &&
                            CAN_ST.sensor_data->R >= (int) MINIMUM_SENSOR_VALUE)    {


                           CAN_ST.motor_data->FRS = (uint8_t) COMMAND_MOTOR_FORWARD;
                           CAN_ST.motor_data->LR = (uint8_t) COMMAND_MOTOR_RIGHT; //turn right
                           CAN_ST.motor_data->SPD = (uint8_t) COMMAND_MOTOR_MEDIUM;

                            current_state_free_run = 3;
                            printf("state %d -> %d\n", prev_state_free_run, current_state_free_run);
                        }

                        //if only right sensor is blocked
                        else if (CAN_ST.sensor_data->L >= (int) MINIMUM_SENSOR_VALUE &&
                               CAN_ST.sensor_data->M >= (int) MINIMUM_SENSOR_BLOCKED_VALUE &&
                               CAN_ST.sensor_data->R < (int) MINIMUM_SENSOR_VALUE)    {

                             //  mast_mot_msg_t *p = (sen_msg_t*) & msg_tx.data.bytes[0];
                            CAN_ST.motor_data->FRS = (uint8_t) COMMAND_MOTOR_FORWARD;
                            CAN_ST.motor_data->LR = (uint8_t) COMMAND_MOTOR_LEFT; //turn left
                            CAN_ST.motor_data->SPD = (uint8_t) COMMAND_MOTOR_MEDIUM;

                               current_state_free_run = 3;
                               printf("state %d -> %d\n", prev_state_free_run, current_state_free_run);
                         }
                       //if clear just go forward
                        else {

                               CAN_ST.motor_data->FRS = (uint8_t) COMMAND_MOTOR_FORWARD;
                               CAN_ST.motor_data->LR = (uint8_t) COMMAND_MOTOR_STRAIGHT; //turn left
                               CAN_ST.motor_data->SPD = (uint8_t) COMMAND_MOTOR_MEDIUM;
                               current_state_free_run = 3;
                              printf("state %d -> %d\n", prev_state_free_run, current_state_free_run);
                        }
                        break;

                    //command the Motor
                    case 3:
                        prev_state_free_run = current_state_free_run;
                        //send our motor commands
                        msg_tx.msg_id = (uint32_t) MASTER_COMMANDS_MOTOR;
                        xQueueSend(scheduler_task::getSharedObject(shared_CAN_message_queue_master), &msg_tx, 0);

                        current_state_free_run = 0; //reset state
                        printf("state %d -> %d\n", prev_state_free_run, current_state_free_run);
                        break;

                    //go to this state if middle sensor is blocked
                    case 4:
                        prev_state_free_run = current_state_free_run;

                        //check if we can back up
                        if(CAN_ST.sensor_data->B >= (int) MINIMUM_SENSOR_BLOCKED_VALUE)    {

                            //if both middle and left sensors are blocked
                            // reverse while turning left
                            if (CAN_ST.sensor_data->L < (int) MINIMUM_SENSOR_VALUE) {
                                CAN_ST.motor_data->FRS = (uint8_t) COMMAND_MOTOR_REVERSE;
                                CAN_ST.motor_data->LR = (uint8_t) COMMAND_MOTOR_LEFT; //turn LEFT
                                CAN_ST.motor_data->SPD = (uint8_t) COMMAND_MOTOR_SLOW;
                            }
                            //reverse while turning right
                            else if (CAN_ST.sensor_data->R < (int) MINIMUM_SENSOR_VALUE)  {
                                CAN_ST.motor_data->FRS = (uint8_t) COMMAND_MOTOR_REVERSE;
                                CAN_ST.motor_data->LR = (uint8_t) COMMAND_MOTOR_RIGHT; //turn RIGHT
                                CAN_ST.motor_data->SPD = (uint8_t) COMMAND_MOTOR_SLOW;
                            }
                            else {
                                //default, reverse while turning left
                                CAN_ST.motor_data->FRS = (uint8_t) COMMAND_MOTOR_REVERSE;
                                CAN_ST.motor_data->LR = (uint8_t) COMMAND_MOTOR_LEFT; //turn LEFT
                                CAN_ST.motor_data->SPD = (uint8_t) COMMAND_MOTOR_SLOW;
                            }
                        }
                        //if both middle and back sensors are blocked, stop the vehicle, light up
                        // some LED, we're stuck, we need help!!!
                        else {

                            CAN_ST.motor_data->FRS = (uint8_t) COMMAND_MOTOR_REVERSE;
                            CAN_ST.motor_data->LR = (uint8_t) COMMAND_MOTOR_LEFT; //turn LEFT
                            CAN_ST.motor_data->SPD = (uint8_t) COMMAND_MOTOR_SLOW;

                        }


                        // No matter what the result is, we go to state 3
                        //Go to state 3 to send our command to motor
                        current_state_free_run = 3;
                        printf("state %d -> %d\n", prev_state_free_run, current_state_free_run);
                        break;

                    default:
                        //Go back to state 0
                        current_state_free_run = 0;
                        break;

                } //end  switch(current_state_free_run)
          //  } //end while( current_state_free_run != 12)

       // }

}

void period_100Hz(void)
{


    //message coming from the terminal
      if(xQueueReceive(scheduler_task::getSharedObject(shared_CAN_message_queue_terminal), &msg_rx, 0) ||
              //message coming from the real CAN receiver
              xQueueReceive(scheduler_task::getSharedObject(shared_CAN_message_queue_receive), &msg_rx, 0) )    {
          //send to control_handler_task
          xQueueSend(scheduler_task::getSharedObject(shared_CAN_message_queue_master), &msg_rx, 0);
      }

      //Handle if no message is received
       else    {
           //msg_rx.msg_id = 0xFFF; //this one should be removed
          // xQueueSend(scheduler_task::getSharedObject(shared_CAN_message_queue_master), &msg_rx, 0);
       }

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


