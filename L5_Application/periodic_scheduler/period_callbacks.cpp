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

#include "243_can/CAN_structs.h"
#include "243_can/iCAN.hpp"

#define DEBUG_NO_CAN 1


/// This is the stack size used for each of the period tasks
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

const int g_reset = 0;
const int g_max_count_timer = 10; // we're running in 100Hz and we expect messages within 10Hz.

//currently not used
uint8_t current_mode = 0;

//motor command values
mast_mot_msg_t motor_commands = { 0 };

//sensor values
sen_msg_t sensor_data = { 0 };

uint8_t reverse_counter = 0;

can_fullcan_msg_t *g_sensor_msg;
can_fullcan_msg_t *g_gps_msg;

//currently not used
can_msg_t msg_rx = { 0 };
can_msg_t msg_tx = { 0 };


//States
static const int CHECK_SENSOR_VALUES = 0;
static const int CONTROL_BASED_ON_SENSOR = 1;
static const int CONTROL_BASED_ON_HEADING = 2;
static const int SEND_CONTROL_TO_MOTOR = 3;

//Sensor readings based on minimum values
bool LEFT_BLOCKED = false;
bool RIGHT_BLOCKED = false;
bool MIDDLE_BLOCKED = false;
bool BACK_BLOCKED = false;

//if all three are clear
bool FRONT_CLEAR = false;

//Commands for motor
static const int COMMAND_MOTOR_STOP = 0;
static const int COMMAND_MOTOR_FORWARD_STRAIGHT = 1;
static const int COMMAND_MOTOR_FORWARD_LEFT = 2;
static const int COMMAND_MOTOR_FORWARD_RIGHT = 3;
static const int COMMAND_MOTOR_REVERSE_LEFT = 4;
static const int COMMAND_MOTOR_REVERSE_RIGHT = 5;


//default values are all zero
//static const sen_msg_t *g_default_sensor_values = new sen_msg_t{(uint8_t) 0x0C, (uint8_t) 0x0D, (uint8_t) 0x0E, (uint8_t) 0x0F};
sen_msg_t *g_default_sensor_values;




can_msg_t msg_rx = { 0 };
can_msg_t msg_tx = { 0 };

//current state
volatile int g_current_state = CHECK_SENSOR_VALUES;

//previous state
volatile int g_prev_state = 0;


//use as heart beat monitor counter
int g_heart_counter = 0;



/// Called once before the RTOS is started, this is a good place to initialize things once
bool period_init(void)
{
    return true; // Must return true upon success
}
uint8_t reverse_counter = 0;

bool init_global = false;


bool g_read_success = false;


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

can_data_t periodic_sensor_data =
{ 0 };

void period_1Hz(void)
{
    LE.toggle(1);

    // printf ("CAN_STORAGE MOTOR VALUE %d\n", CAN_ST.getMotorSpeed());
    //sen_value = scheduler_task::getSharedObject(shared_sensor_data);

void printMotorCommand(int print_command)    {

    switch(print_command) {
           case COMMAND_MOTOR_STOP:
              printf("COMMAND_MOTOR_STOP\n");
               break;

           case COMMAND_MOTOR_FORWARD_STRAIGHT:
               printf("COMMAND_MOTOR_FORWARD_STRAIGHT\n");
               break;

           case COMMAND_MOTOR_FORWARD_LEFT:
               printf("COMMAND_MOTOR_FORWARD_LEFT\n");
               break;


           case COMMAND_MOTOR_FORWARD_RIGHT:
               printf("COMMAND_MOTOR_FORWARD_RIGHT\n");
               break;

           case COMMAND_MOTOR_REVERSE_LEFT:
               printf("COMMAND_MOTOR_REVERSE_LEFT\n");
               break;

           case COMMAND_MOTOR_REVERSE_RIGHT:
               printf("COMMAND_MOTOR_REVERSE_RIGHT\n");
               break;
           default:
               printf("COMMAND_MOTOR_REVERSE_RIGHT\n");
               break;
       }
}


void period_10Hz(void)
{

    //reset freerun state counter
    // current_state_free_run = 0;

void parseSensorReading(sen_msg_t* data)  {


    if (data->L < MINIMUM_SENSOR_VALUE ) LEFT_BLOCKED = true;
        else LEFT_BLOCKED = false;

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


    // CAN_ST.sensor_data = (sen_msg_t*) & periodic_sensor_data.bytes[0];
    // if (current_mode == FREERUN_MODE)   {
    //state machine for free run
    // while( current_state_free_run != 12) {
    switch(current_state_free_run)
    {

    if (data->M < MINIMUM_SENSOR_BLOCKED_VALUE ) MIDDLE_BLOCKED = true;
        else MIDDLE_BLOCKED = false;


    if (data->R < MINIMUM_SENSOR_VALUE ) RIGHT_BLOCKED = true;
       else RIGHT_BLOCKED = false;

    if (data->L < MINIMUM_SENSOR_VALUE ) LEFT_BLOCKED = true;
       else LEFT_BLOCKED = false;


    if (data->B < MINIMUM_SENSOR_BLOCKED_VALUE ) BACK_BLOCKED = true;
       else BACK_BLOCKED = false;

    if (!LEFT_BLOCKED && !MIDDLE_BLOCKED && !RIGHT_BLOCKED) FRONT_CLEAR = true;
    else FRONT_CLEAR = false;

}

void generateMotorCommands(int command)    {

    switch(command) {
        case COMMAND_MOTOR_STOP:
            CAN_ST.motor_data->FRS = (uint8_t) COMMAND_STOP;
            CAN_ST.motor_data->LR = (uint8_t) COMMAND_STRAIGHT;
            CAN_ST.motor_data->SPD = (uint8_t) COMMAND_SLOW;

            break;

        case COMMAND_MOTOR_FORWARD_STRAIGHT:
            CAN_ST.motor_data->FRS = (uint8_t) COMMAND_FORWARD;
            CAN_ST.motor_data->LR = (uint8_t) COMMAND_STRAIGHT;
            CAN_ST.motor_data->SPD = (uint8_t) COMMAND_MEDIUM;
            break;

        case COMMAND_MOTOR_FORWARD_LEFT:
            CAN_ST.motor_data->FRS = (uint8_t) COMMAND_FORWARD;
            CAN_ST.motor_data->LR = (uint8_t) COMMAND_LEFT;
            CAN_ST.motor_data->SPD = (uint8_t) COMMAND_MEDIUM;
            break;

        case COMMAND_MOTOR_FORWARD_RIGHT:
            CAN_ST.motor_data->FRS = (uint8_t) COMMAND_FORWARD;
            CAN_ST.motor_data->LR = (uint8_t) COMMAND_RIGHT;
            CAN_ST.motor_data->SPD = (uint8_t) COMMAND_MEDIUM;
            break;

        case COMMAND_MOTOR_REVERSE_LEFT:
            CAN_ST.motor_data->FRS = (uint8_t) COMMAND_REVERSE;
            CAN_ST.motor_data->LR = (uint8_t) COMMAND_LEFT;
            CAN_ST.motor_data->SPD = (uint8_t) COMMAND_SLOW;
            break;

        case COMMAND_MOTOR_REVERSE_RIGHT:
            CAN_ST.motor_data->FRS = (uint8_t) COMMAND_REVERSE;
            CAN_ST.motor_data->LR = (uint8_t) COMMAND_RIGHT;
            CAN_ST.motor_data->SPD = (uint8_t) COMMAND_SLOW;
            break;
        default:
            CAN_ST.motor_data->FRS = (uint8_t) COMMAND_STOP;
            CAN_ST.motor_data->LR = (uint8_t) COMMAND_STRAIGHT;
            CAN_ST.motor_data->SPD = (uint8_t) COMMAND_SLOW;
            break;
    }
    printMotorCommand(command);


}


void period_1Hz(void)
{
    LE.toggle(1);
    printf("HEART BEAT: %d\n", g_heart_counter++);
    if (10000 < g_heart_counter) g_heart_counter = 0;

}

//counter for the receive sensor
int g_sensor_receive_counter = 0;

void period_10Hz(void)
{

    //local sensor values
   //sen_msg_t *l_sensor_values = new sen_msg_t{0};
    portDISABLE_INTERRUPTS();
    //copy our global values to local values
    //sen_msg_t *l_sensor_values = (sen_msg_t*) &CAN_ST.sensor_data;
    sen_msg_t *l_sensor_values = CAN_ST.sensor_data;
    portENABLE_INTERRUPTS();


    uint8_t temp[4];

    // LE.toggle(2);

    CAN_ST.motor_data = (mast_mot_msg_t*) & msg_tx.data.bytes[0];
  // printf("%d %d %d %d\n", g_default_sensor_values->L,g_default_sensor_values->M, g_default_sensor_values->R, g_default_sensor_values->B);
    //printf("%d %d %d %d\n", temp[0], temp[1], temp[2], temp[3]);

    switch(g_current_state){

        case CHECK_SENSOR_VALUES:
            g_prev_state = g_current_state;

                //testing
                temp[0] = (uint8_t) l_sensor_values->L;
                temp[1] = (uint8_t) l_sensor_values->M;
                temp[2] = (uint8_t) l_sensor_values->R;
                temp[3] = (uint8_t) l_sensor_values->B;
                printf("%d %d %d %d\n", temp[0], temp[1], temp[2], temp[3]);

                parseSensorReading(l_sensor_values);
                if(FRONT_CLEAR) g_current_state = CONTROL_BASED_ON_HEADING;
                else g_current_state = CONTROL_BASED_ON_SENSOR;
            break;

        case CONTROL_BASED_ON_SENSOR:
            g_prev_state = g_current_state;

            // only left is blocked
           if (LEFT_BLOCKED && !RIGHT_BLOCKED && !MIDDLE_BLOCKED)  {
                  generateMotorCommands(COMMAND_MOTOR_FORWARD_RIGHT); //turn right
           }
           // only right is blocked
           else if(RIGHT_BLOCKED && !LEFT_BLOCKED && !MIDDLE_BLOCKED)   {
               generateMotorCommands(COMMAND_MOTOR_FORWARD_LEFT); //turn left
           }
           //only middle is blocked
           else if(MIDDLE_BLOCKED && !LEFT_BLOCKED && !RIGHT_BLOCKED)    {
               generateMotorCommands(COMMAND_MOTOR_FORWARD_RIGHT); //turn right
           }
           //if left and middle is blocked
           else if (LEFT_BLOCKED && MIDDLE_BLOCKED && !RIGHT_BLOCKED)   {
               generateMotorCommands(COMMAND_MOTOR_FORWARD_RIGHT);  //turn right
           }
           //if right and middle is blocked
           else if (RIGHT_BLOCKED && MIDDLE_BLOCKED && !LEFT_BLOCKED)   {
               generateMotorCommands(COMMAND_MOTOR_FORWARD_LEFT); //turn left
           }
           //if all front sensors are blocked
           else if (!FRONT_CLEAR)   {
               //if back sensor is also blocked
               if (BACK_BLOCKED)    {
                   generateMotorCommands(COMMAND_STOP);
                   LD.setLeftDigit('E');
                   LD.setRightDigit('R');
               }
               //if back is clear, reverse-left
               else {
                   generateMotorCommands(COMMAND_MOTOR_REVERSE_LEFT);
               }
           }
            //    if (forward_blocked) {
            //        if (no_clearance) {
            //            motor_cmd = reverse;
            //        }
            //        else {
            //            steer_cmd = left OR right;
            //        }
            //    }
            //    else if (left_blocked) {
            //        steer_cmd = steer_right;
            //    }
            //    else if (right_blocked) {
            //        steer_cmd = steer_left;
            //    }

            g_current_state = SEND_CONTROL_TO_MOTOR;
            break;

        case CONTROL_BASED_ON_HEADING:
            g_prev_state = g_current_state;

            //this is where we command based on the heading

            // now we're testing without GPS
            // so we go straight
            generateMotorCommands(COMMAND_MOTOR_FORWARD_STRAIGHT);

            g_current_state = SEND_CONTROL_TO_MOTOR;


            //g_current_state = SEND_CONTROL_TO_MOTOR;
            break;

        case SEND_CONTROL_TO_MOTOR:
            g_prev_state = g_current_state;
            printf("SENDING FRS:%2x LR:%2x SPD:%2x\n", msg_tx.data.bytes[0],  msg_tx.data.bytes[1],  msg_tx.data.bytes[2] );
            //prepare our message id
            msg_tx.msg_id = (uint32_t) MASTER_COMMANDS_MOTOR;
            //send our message
            if(iCAN_tx(&msg_tx, (uint16_t) MASTER_COMMANDS_MOTOR))   {
                printf("Message sent to motor!\n");
            }

            g_current_state = CHECK_SENSOR_VALUES;
            break;

        default:
            g_current_state = CHECK_SENSOR_VALUES;
            break;



        default:
        //Go back to state 0
        current_state_free_run = 0;
        break;

    } //end  switch(current_state_free_run)
    //  } //end while( current_state_free_run != 12)

    // }



//    if (forward_blocked) {
//        if (no_clearance) {
//            motor_cmd = reverse;
//        }
//        else {
//            steer_cmd = left OR right;
//        }
//    }
//    else if (left_blocked) {
//        steer_cmd = steer_right;
//    }
//    else if (right_blocked) {
//        steer_cmd = steer_left;
//    }

}

    } //end switch(g_current_state)


    delete l_sensor_values;

} // end 10Hz task


void period_100Hz(void)
{


    //message coming from the terminal
    if (xQueueReceive(
            scheduler_task::getSharedObject(shared_CAN_message_queue_terminal),
            &msg_rx, 0) ||
            //message coming from the real CAN receiver
            xQueueReceive(scheduler_task::getSharedObject(shared_CAN_message_queue_receive), &msg_rx, 0))
    {
        //send to control_handler_task
        xQueueSend(
                scheduler_task::getSharedObject(
                        shared_CAN_message_queue_master), &msg_rx, 0);
    }

    //can_fullcan_msg_t *temp_rx = new can_fullcan_msg_t;
    can_fullcan_msg_t *temp_rx = new can_fullcan_msg_t{0};
        /**
        * pARSING TASK
        *      can_msg_t   sensor_msg;
        *      can_msg_t   geo_msg;
        *
        *     This task will just copy the incoming message into appropriate can_msg_t
        *
        *     portDISABLE_INTERRUPTS();
        *     sensor_msg = mCAN_MSG_Rx;
        *     portENABLE_INTERRPTS();
        *
        * On the reader side:
        *      disable_ints();
        *      can_msg_t sensor_msg = THIS_CLASS::sensor_msg;
        *      enable_ints();
        *
        *      sensor_msg_ptr = (sensor_msg_ptr) * sensor_msg;
        *
        *      If a message never arrives, you can basically do this:
        *      sensor_msg = sensor_msg_safe_copy;
        *
        */

//        can_fullcan_msg_t *default_sensor_msg = new can_fullcan_msg_t{0};
//        default_sensor_msg->data.bytes[0] = (uint8_t) 1;
//        default_sensor_msg->data.bytes[1] = (uint8_t) 2;
//        default_sensor_msg->data.bytes[2] = (uint8_t) 3;
//        default_sensor_msg->data.bytes[3] = (uint8_t) 4;

        //RECEIVE AND SAVE FULL_CAN MESSAGES
        if(iCAN_rx(temp_rx, (uint16_t) SENSOR_MASTER_REG))    {
            portDISABLE_INTERRUPTS();
           // g_sensor_msg = &temp_rx;
            CAN_ST.sensor_data = (sen_msg_t*) & temp_rx->data.bytes[0];
//            CAN_ST.sensor_data->L = temp_rx.data.bytes[0];
//            CAN_ST.sensor_data->M = temp_rx.data.bytes[1];
//            CAN_ST.sensor_data->R = temp_rx.data.bytes[2];
//            CAN_ST.sensor_data->B = temp_rx.data.bytes[3];
            portENABLE_INTERRUPTS();

            printf("SENSOR VAL READ!\n");
            //g_reset counter because we received a message
            g_sensor_receive_counter = g_reset;
        }
        //if no message arrives
#if !DEBUG_NO_CAN
        else if (g_sensor_receive_counter > g_max_count_timer)  {

                portDISABLE_INTERRUPTS();
                CAN_ST.setSafeSensorValues();
                //CAN_ST.sensor_data = (sen_msg_t*) & default_sensor_msg->data.bytes[0];
                //g_sensor_msg = &default_sensor_msg;
//                CAN_ST.sensor_data->L = 2;
//                CAN_ST.sensor_data->M = 3;
//                CAN_ST.sensor_data->R = 4;
//                CAN_ST.sensor_data->B = 5;
                portENABLE_INTERRUPTS();

                g_sensor_receive_counter = g_reset;
           // printf("SENSOR VAL RESET! %d\n", g_sensor_receive_counter);
        }
#endif
>>>>>>> implemented prototype logic, unclean

        //we increment counter
        g_sensor_receive_counter++;

    delete temp_rx;
    //delete default_sensor_msg;

#if DEBUG_NO_CAN
    // LE.toggle(3);
    int sw_value = SW.getSwitchValues();
    switch(sw_value)    {
        case 0:
            break;
        case 1:
            portDISABLE_INTERRUPTS();
            CAN_ST.sensor_data->L = (uint8_t) 0x00;
            CAN_ST.sensor_data->M = (uint8_t) 0xff;
            CAN_ST.sensor_data->R = (uint8_t) 0xff;
            CAN_ST.sensor_data->B = (uint8_t) 0xff;
            portENABLE_INTERRUPTS();
            break;
        case 2:
            portDISABLE_INTERRUPTS();
            CAN_ST.sensor_data->L = (uint8_t) 0xff;
            CAN_ST.sensor_data->M = (uint8_t) 0x00;
            CAN_ST.sensor_data->R = (uint8_t) 0xff;
            CAN_ST.sensor_data->B = (uint8_t) 0xff;
            portENABLE_INTERRUPTS();
            break;
        case 3:
            portDISABLE_INTERRUPTS();
            CAN_ST.sensor_data->L = (uint8_t) 0x00;
            CAN_ST.sensor_data->M = (uint8_t) 0x00;
            CAN_ST.sensor_data->R = (uint8_t) 0xff;
            CAN_ST.sensor_data->B = (uint8_t) 0xff;
            portENABLE_INTERRUPTS();
            break;
        case 4:
            portDISABLE_INTERRUPTS();
            CAN_ST.sensor_data->L = (uint8_t) 0xff;
            CAN_ST.sensor_data->M = (uint8_t) 0xff;
            CAN_ST.sensor_data->R = (uint8_t) 0x00;
            CAN_ST.sensor_data->B = (uint8_t) 0xff;
            portENABLE_INTERRUPTS();
            break;
        case 5:
            portDISABLE_INTERRUPTS();
            CAN_ST.sensor_data->L = (uint8_t) 0x00;
            CAN_ST.sensor_data->M = (uint8_t) 0xff;
            CAN_ST.sensor_data->R = (uint8_t) 0x00;
            CAN_ST.sensor_data->B = (uint8_t) 0xff;
            portENABLE_INTERRUPTS();
            break;
        case 6:
            portDISABLE_INTERRUPTS();
            CAN_ST.sensor_data->L = (uint8_t) 0xff;
            CAN_ST.sensor_data->M = (uint8_t) 0x00;
            CAN_ST.sensor_data->R = (uint8_t) 0x00;
            CAN_ST.sensor_data->B = (uint8_t) 0xff;
            portENABLE_INTERRUPTS();
            break;
        case 7:
            portDISABLE_INTERRUPTS();
            CAN_ST.sensor_data->L = (uint8_t) 0x00;
            CAN_ST.sensor_data->M = (uint8_t) 0x00;
            CAN_ST.sensor_data->R = (uint8_t) 0x00;
            CAN_ST.sensor_data->B = (uint8_t) 0xff;
            portENABLE_INTERRUPTS();
            break;
        case 8:
            portDISABLE_INTERRUPTS();
            CAN_ST.sensor_data->L = (uint8_t) 0xff;
            CAN_ST.sensor_data->M = (uint8_t) 0xff;
            CAN_ST.sensor_data->R = (uint8_t) 0xff;
            CAN_ST.sensor_data->B = (uint8_t) 0x00;
            portENABLE_INTERRUPTS();
            break;

        case 15:
            portDISABLE_INTERRUPTS();
            CAN_ST.sensor_data->L = (uint8_t) 0x00;
            CAN_ST.sensor_data->M = (uint8_t) 0x00;
            CAN_ST.sensor_data->R = (uint8_t) 0x00;
            CAN_ST.sensor_data->B = (uint8_t) 0x00;
            break;

        default:
            break;
    }
#endif
}

void period_1000Hz(void)
{

    //LE.toggle(4);

}

void handleMessage()
{

}

