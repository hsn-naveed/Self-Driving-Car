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

#include "243_can/CAN_structs.h"
#include "243_can/iCAN.hpp"

/// This is the stack size used for each of the period tasks
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

// set to 1 if you want to enable motor_control with switches
// set to 0 if you want to use CAN
#define DEBUG_NO_CAN 1



//States
static const int CHECK_SENSOR_VALUES = 0;
static const int CONTROL_BASED_ON_SENSOR = 1;
static const int CONTROL_BASED_ON_HEADING = 2;
static const int SEND_CONTROL_TO_MOTOR = 3;
static const int STATE_MACHINE_END = 4;

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

//for 7LED display
static const int DISPLAY_FORWARD_STRAIGHT = 0;
static const int DISPLAY_FORWARD_LEFT = 1;
static const int DISPLAY_FORWARD_RIGHT = 2;
static const int DISPLAY_STOP = 3;
static const int DISPLAY_REVERSE = 4;
static const int DISPLAY_ERROR = 99;


const int g_reset = 0;
const int g_max_count_timer = 300; // we're running in 100Hz and we expect messages within 3Hz.

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


//default values are all zero
//static const sen_msg_t *g_default_sensor_values = new sen_msg_t{(uint8_t) 0x0C, (uint8_t) 0x0D, (uint8_t) 0x0E, (uint8_t) 0x0F};
sen_msg_t *g_default_sensor_values;


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

/// Register any telemetry variables
bool period_reg_tlm(void)
{
    // Make sure "SYS_CFG_ENABLE_TLM" is enabled at sys_config.h to use Telemetry
    return true; // Must return true upon success
}


void disp_7LED(int val)  {
    switch(val) {
        case DISPLAY_FORWARD_STRAIGHT:
            LD.clear();
            LD.setLeftDigit('1');
            LD.setRightDigit('1');
            break;


        case DISPLAY_FORWARD_LEFT:
            LD.clear();
            LD.setLeftDigit('L');
            LD.setRightDigit('E');
            break;


        case DISPLAY_FORWARD_RIGHT:
            LD.clear();
            LD.setLeftDigit('-');
            LD.setRightDigit('A');
            break;

        case DISPLAY_STOP:
            LD.clear();
            LD.setLeftDigit('S');
            LD.setRightDigit('S');
            break;

        case DISPLAY_REVERSE:
            LD.clear();
            LD.setLeftDigit('6');
            LD.setRightDigit('9');
            break;

        case DISPLAY_ERROR:
            LD.clear();
            LD.setLeftDigit('E');
            LD.setRightDigit('E');
            break;

        default:
            LD.clear();
            LD.setLeftDigit('8');
            LD.setRightDigit('8');
            break;
    }

} //end disp_LED


void printMotorCommand(int print_command)    {

    switch(print_command) {
           case COMMAND_MOTOR_STOP:
             // printf("COMMAND_MOTOR_STOP\n");
               break;

           case COMMAND_MOTOR_FORWARD_STRAIGHT:
               //printf("COMMAND_MOTOR_FORWARD_STRAIGHT\n");
               break;

           case COMMAND_MOTOR_FORWARD_LEFT:
              // printf("COMMAND_MOTOR_FORWARD_LEFT\n");
               break;

           case COMMAND_MOTOR_FORWARD_RIGHT:
              // printf("COMMAND_MOTOR_FORWARD_RIGHT\n");
               break;

           case COMMAND_MOTOR_REVERSE_LEFT:
              // printf("COMMAND_MOTOR_REVERSE_LEFT\n");
               break;

           case COMMAND_MOTOR_REVERSE_RIGHT:
               //printf("COMMAND_MOTOR_REVERSE_RIGHT\n");
               break;
           default:
              // printf("COMMAND_MOTOR_REVERSE_RIGHT\n");
               break;
       }
}

void parseSensorReading(sen_msg_t* data)  {


    if (data->L < MINIMUM_SENSOR_VALUE ) LEFT_BLOCKED = true;
        else LEFT_BLOCKED = false;

    if (data->M < MINIMUM_SENSOR_BLOCKED_VALUE ) MIDDLE_BLOCKED = true;
        else MIDDLE_BLOCKED = false;

    if (data->R < MINIMUM_SENSOR_VALUE ) RIGHT_BLOCKED = true;
       else RIGHT_BLOCKED = false;

    if (data->B < MINIMUM_SENSOR_BLOCKED_VALUE ) BACK_BLOCKED = true;
       else BACK_BLOCKED = false;

    if (!LEFT_BLOCKED && !MIDDLE_BLOCKED && !RIGHT_BLOCKED) FRONT_CLEAR = true;
    else FRONT_CLEAR = false;

}

void generateMotorCommands(int command)    {

    switch(command) {
        case COMMAND_MOTOR_STOP:

            CAN_ST.motor_data->LR = (uint8_t) COMMAND_STRAIGHT;
            CAN_ST.motor_data->SPD = (uint8_t) COMMAND_STOP;
            disp_7LED(DISPLAY_STOP);
            break;


        case COMMAND_MOTOR_FORWARD_STRAIGHT:

            CAN_ST.motor_data->LR = (uint8_t) COMMAND_STRAIGHT;
            CAN_ST.motor_data->SPD = (uint8_t) COMMAND_MEDIUM;
            disp_7LED(DISPLAY_FORWARD_STRAIGHT);
            break;


        case COMMAND_MOTOR_FORWARD_LEFT:

            CAN_ST.motor_data->LR = (uint8_t) COMMAND_LEFT;
            CAN_ST.motor_data->SPD = (uint8_t) COMMAND_MEDIUM;
            disp_7LED(DISPLAY_FORWARD_LEFT);
            break;


        case COMMAND_MOTOR_FORWARD_RIGHT:

            CAN_ST.motor_data->LR = (uint8_t) COMMAND_RIGHT;
            CAN_ST.motor_data->SPD = (uint8_t) COMMAND_MEDIUM;
            disp_7LED(DISPLAY_FORWARD_RIGHT);
            break;

        case COMMAND_MOTOR_REVERSE_LEFT:

            CAN_ST.motor_data->LR = (uint8_t) COMMAND_LEFT;
            CAN_ST.motor_data->SPD = (uint8_t) COMMAND_STOP; //change this for reverse!
            disp_7LED(DISPLAY_REVERSE);
            break;

        case COMMAND_MOTOR_REVERSE_RIGHT:

            CAN_ST.motor_data->LR = (uint8_t) COMMAND_RIGHT;
            CAN_ST.motor_data->SPD = (uint8_t) COMMAND_STOP; //change this for reverse!
            disp_7LED(DISPLAY_REVERSE);
            break;
        default:

            CAN_ST.motor_data->LR = (uint8_t) COMMAND_STRAIGHT;
            CAN_ST.motor_data->SPD = (uint8_t) COMMAND_STOP;
            disp_7LED(DISPLAY_STOP);
            break;
    }
    printMotorCommand(command);


}


void period_1Hz(void)
{
    LE.toggle(1);
    printf("\n\n\nHEART BEAT: %d\n\n\n", g_heart_counter++);
    if (10000 < g_heart_counter) g_heart_counter = 0;

}

//counter for the receive sensor
int g_sensor_receive_counter = 0;

void period_10Hz(void)
{

   // delete l_sensor_values;

} // end 10Hz task



void period_100Hz(void)
{


    //can_fullcan_msg_t *temp_rx = new can_fullcan_msg_t;


    can_fullcan_msg_t *temp_rx = new can_fullcan_msg_t{0};

   // can_fullcan_msg_t *temp_rx;
        //RECEIVE AND SAVE FULL_CAN MESSAGES
        if(iCAN_rx(temp_rx, (uint16_t) SENSOR_MASTER_REG))    {

            portDISABLE_INTERRUPTS();

          //  CAN_ST.sensor_data = (sen_msg_t*) & temp_rx->data.bytes[0];
            //if this ^ does not work try to uncomment these:
        CAN_ST.sensor_data->L = (uint8_t) temp_rx->data.bytes[0];
        CAN_ST.sensor_data->M =(uint8_t) temp_rx->data.bytes[1];
        CAN_ST.sensor_data->R = (uint8_t) temp_rx->data.bytes[2];
        CAN_ST.sensor_data->B =  (uint8_t) temp_rx->data.bytes[3];

            portENABLE_INTERRUPTS();

            printf("SENSOR VAL READ!\n");
           // printf("NEW VALUES: %d\n", (int) CAN_ST.sensor_data->L);
           // printf("NEW VALUES %d %d %d %d\n", (int) CAN_ST.sensor_data->L, (int) CAN_ST.sensor_data->M, (int) CAN_ST.sensor_data->R, (int) CAN_ST.sensor_data->B );
            //g_reset counter because we received a message
            g_sensor_receive_counter = g_reset;
        }
        //GPS sends the current coordinates of the car
        //In the final stage, add the logic that motor does not start until coordinates are received
        else if(iCAN_rx(temp_rx, (uint16_t) GPS_MASTER_COORDS)){
            portDISABLE_INTERRUPTS();
            CAN_ST.gps_coords_curr = (gps_coordinate_msg_t*) &temp_rx->data.qword;
            portENABLE_INTERRUPTS();
            printf("GPS VAL READ!\n");
            g_sensor_receive_counter = g_reset;
        }
        //Compass sending current heading
        else if(iCAN_rx(temp_rx, (uint16_t) GPS_MASTER_HEADING)){
                    portDISABLE_INTERRUPTS();
                    CAN_ST.mAngleValue = (gps_heading_msg_t*) &temp_rx->data.qword;
                    portENABLE_INTERRUPTS();
                    printf("Heading READ!\n");
                    g_sensor_receive_counter = g_reset;
                }


        //if no message arrives
#if !DEBUG_NO_CAN
        else if (g_sensor_receive_counter > g_max_count_timer)  {

                portDISABLE_INTERRUPTS();
                    CAN_ST.setSafeSensorValues();
                portENABLE_INTERRUPTS();

                //reset our counter because we fill-in safe values
                g_sensor_receive_counter = g_reset;
        }
#endif

        //we increment counter
        g_sensor_receive_counter++;


        ///////////////////////////////////////////////////////////////

#if DEBUG_NO_CAN == 1
    // LE.toggle(3);
    int sw_value = SW.getSwitchValues();
    switch(sw_value)    {
        case 0:
            portDISABLE_INTERRUPTS();
            temp_rx->data.bytes[0] = (uint8_t) 0xff;
            temp_rx->data.bytes[1] = (uint8_t) 0xff;
            temp_rx->data.bytes[2] = (uint8_t) 0xff;
            temp_rx->data.bytes[3] = (uint8_t) 0xff;
            CAN_ST.sensor_data = (sen_msg_t*) & temp_rx->data.bytes[0];
            portENABLE_INTERRUPTS();
            break;

        case 1:
            portDISABLE_INTERRUPTS();
            temp_rx->data.bytes[0] = (uint8_t) 0x00;
            temp_rx->data.bytes[1] = (uint8_t) 0xff;
            temp_rx->data.bytes[2] = (uint8_t) 0xff;
            temp_rx->data.bytes[3] = (uint8_t) 0xff;
            CAN_ST.sensor_data = (sen_msg_t*) & temp_rx->data.bytes[0];
            portENABLE_INTERRUPTS();
            break;

        case 2:
            portDISABLE_INTERRUPTS();
            temp_rx->data.bytes[0] = (uint8_t) 0xff;
            temp_rx->data.bytes[1] = (uint8_t) 0x00;
            temp_rx->data.bytes[2] = (uint8_t) 0xff;
            temp_rx->data.bytes[3] = (uint8_t) 0xff;
            CAN_ST.sensor_data = (sen_msg_t*) & temp_rx->data.bytes[0];
            portENABLE_INTERRUPTS();
            break;

        case 3:
            portDISABLE_INTERRUPTS();
            temp_rx->data.bytes[0] = (uint8_t) 0x00;
            temp_rx->data.bytes[1] = (uint8_t) 0x00;
            temp_rx->data.bytes[2] = (uint8_t) 0xff;
            temp_rx->data.bytes[3] = (uint8_t) 0xff;
            CAN_ST.sensor_data = (sen_msg_t*) & temp_rx->data.bytes[0];
            portENABLE_INTERRUPTS();
            break;

        case 4:
            portDISABLE_INTERRUPTS();
            temp_rx->data.bytes[0] = (uint8_t) 0xff;
            temp_rx->data.bytes[1] = (uint8_t) 0xff;
            temp_rx->data.bytes[2] = (uint8_t) 0x00;
            temp_rx->data.bytes[3] = (uint8_t) 0xff;
            CAN_ST.sensor_data = (sen_msg_t*) & temp_rx->data.bytes[0];
            portENABLE_INTERRUPTS();
            break;

        case 5:
            portDISABLE_INTERRUPTS();
            temp_rx->data.bytes[0] = (uint8_t) 0x00;
            temp_rx->data.bytes[1] = (uint8_t) 0xff;
            temp_rx->data.bytes[2] = (uint8_t) 0x00;
            temp_rx->data.bytes[3] = (uint8_t) 0xff;
            CAN_ST.sensor_data = (sen_msg_t*) & temp_rx->data.bytes[0];
            portENABLE_INTERRUPTS();
            break;

        case 6:
            portDISABLE_INTERRUPTS();
            temp_rx->data.bytes[0] = (uint8_t) 0xff;
            temp_rx->data.bytes[1] = (uint8_t) 0x00;
            temp_rx->data.bytes[2] = (uint8_t) 0x00;
            temp_rx->data.bytes[3] = (uint8_t) 0xff;
            CAN_ST.sensor_data = (sen_msg_t*) & temp_rx->data.bytes[0];
            portENABLE_INTERRUPTS();
            break;

        case 7:
            portDISABLE_INTERRUPTS();
            temp_rx->data.bytes[0] = (uint8_t) 0x00;
            temp_rx->data.bytes[1] = (uint8_t) 0x00;
            temp_rx->data.bytes[2] = (uint8_t) 0x00;
            temp_rx->data.bytes[3] = (uint8_t) 0xff;
            CAN_ST.sensor_data = (sen_msg_t*) & temp_rx->data.bytes[0];
            portENABLE_INTERRUPTS();
            break;

        case 8:
            portDISABLE_INTERRUPTS();
            temp_rx->data.bytes[0] = (uint8_t) 0xff;
            temp_rx->data.bytes[1] = (uint8_t) 0xff;
            temp_rx->data.bytes[2] = (uint8_t) 0xff;
            temp_rx->data.bytes[3] = (uint8_t) 0x00;
            CAN_ST.sensor_data = (sen_msg_t*) & temp_rx->data.bytes[0];
            portENABLE_INTERRUPTS();
            break;

        case 15:
            portDISABLE_INTERRUPTS();
            temp_rx->data.bytes[0] = (uint8_t) 0x00;
            temp_rx->data.bytes[1] = (uint8_t) 0x00;
            temp_rx->data.bytes[2] = (uint8_t) 0x00;
            temp_rx->data.bytes[3] = (uint8_t) 0x00;
            CAN_ST.sensor_data = (sen_msg_t*) & temp_rx->data.bytes[0];
            portENABLE_INTERRUPTS();
            break;

        default:
            portDISABLE_INTERRUPTS();
            temp_rx->data.bytes[0] = (uint8_t) 0x00;
            temp_rx->data.bytes[1] = (uint8_t) 0x00;
            temp_rx->data.bytes[2] = (uint8_t) 0x00;
            temp_rx->data.bytes[3] = (uint8_t) 0x00;
            CAN_ST.sensor_data = (sen_msg_t*) & temp_rx->data.bytes[0];
            portENABLE_INTERRUPTS();
            break;
    }
#endif

        ///////////////////////////////////////////////////////////////
        sen_msg_t *l_sensor_values;

           portDISABLE_INTERRUPTS();
           //copy our global values to local values
           //sen_msg_t *l_sensor_values = CAN_ST.sensor_data;

           l_sensor_values = CAN_ST.sensor_data;
           //printf("COPY TOOK PLACE %d %d %d %d\n", (int) CAN_ST.sensor_data->L, (int) CAN_ST.sensor_data->M, (int) CAN_ST.sensor_data->R, (int) CAN_ST.sensor_data->B );
           portENABLE_INTERRUPTS();

           //for testing
           uint8_t temp[4];


           CAN_ST.motor_data = (mast_mot_msg_t*) & msg_tx.data.bytes[0];

           g_current_state = 0;

           while(g_current_state != STATE_MACHINE_END) {


               switch(g_current_state){

                   case CHECK_SENSOR_VALUES:
                       g_prev_state = g_current_state;

                       //testing
                       temp[0] = (uint8_t) l_sensor_values->L;
                       temp[1] = (uint8_t) l_sensor_values->M;
                       temp[2] = (uint8_t) l_sensor_values->R;
                       temp[3] = (uint8_t) l_sensor_values->B;
                      // printf("            L: %d   M: %d   R: %d   B: %d\n", temp[0], temp[1], temp[2], temp[3]);

                       //we parse the sensor values
                       parseSensorReading(l_sensor_values);

                       // if all are clear, we follow the heading
                       // else we act based on sensor values
                       if(FRONT_CLEAR) g_current_state = CONTROL_BASED_ON_HEADING;
                       else g_current_state = CONTROL_BASED_ON_SENSOR;

                       break;

                   case CONTROL_BASED_ON_SENSOR:
                       g_prev_state = g_current_state;

                       // only left is blocked         !l m r = -->
                       if (LEFT_BLOCKED && !RIGHT_BLOCKED && !MIDDLE_BLOCKED)  {
                             generateMotorCommands(COMMAND_MOTOR_FORWARD_RIGHT); //turn right

                      }
                      // only right is blocked
                      else if(RIGHT_BLOCKED && !LEFT_BLOCKED && !MIDDLE_BLOCKED)   {
                          generateMotorCommands(COMMAND_MOTOR_FORWARD_LEFT); //turn left

                      }
                      //only middle is blocked
                      else if(MIDDLE_BLOCKED && !LEFT_BLOCKED && !RIGHT_BLOCKED)    {
                        //  generateMotorCommands(COMMAND_MOTOR_FORWARD_RIGHT); //turn right
                          generateMotorCommands(COMMAND_MOTOR_STOP);

                      }
                      //if left and middle is blocked
                      else if (LEFT_BLOCKED && MIDDLE_BLOCKED && !RIGHT_BLOCKED)   {
                          //generateMotorCommands(COMMAND_MOTOR_FORWARD_RIGHT);  //turn right
                          generateMotorCommands(COMMAND_MOTOR_STOP);

                      }
                      //if right and middle is blocked
                      else if (RIGHT_BLOCKED && MIDDLE_BLOCKED && !LEFT_BLOCKED)   {
                          //generateMotorCommands(COMMAND_MOTOR_FORWARD_LEFT); //turn left
                          generateMotorCommands(COMMAND_MOTOR_STOP);

                      }
                      //if left and right are blocked
                      else if(LEFT_BLOCKED && RIGHT_BLOCKED && !MIDDLE_BLOCKED){
                          //generateMotorCommands(COMMAND_MOTOR_FORWARD_STRAIGHT); //keep going straight
                          generateMotorCommands(COMMAND_MOTOR_STOP);

                      }
                      //if all front sensors are blocked
                      else if (!FRONT_CLEAR)   {
                          //if back sensor is also blocked
                          if (BACK_BLOCKED)    {
                              generateMotorCommands(COMMAND_MOTOR_STOP);
                              disp_7LED(DISPLAY_ERROR);
                          }
                          //if back is clear, reverse-left
                          else {
                              generateMotorCommands(COMMAND_MOTOR_REVERSE_LEFT);

                          }
                      }
                      else {
                          generateMotorCommands(COMMAND_MOTOR_STOP);

                      }

                       g_current_state = SEND_CONTROL_TO_MOTOR;

                      break;

                   case CONTROL_BASED_ON_HEADING:
                       g_prev_state = g_current_state;

                       //this is where we command based on the heading
                       //TO DO: GPS IMPLEMENTATION

                       // now we're testing without GPS
                       // so we go straight
                       generateMotorCommands(COMMAND_MOTOR_FORWARD_STRAIGHT);

                       //send our commands
                       g_current_state = SEND_CONTROL_TO_MOTOR;

                       break;

                   case SEND_CONTROL_TO_MOTOR:
                       g_prev_state = g_current_state;

                       //for debugging
                      // printf("SENDING FRS:%2x LR:%2x SPD:%2x\n", msg_tx.data.bytes[0],  msg_tx.data.bytes[1],  msg_tx.data.bytes[2] );

                       //prepare our message id
                       msg_tx.msg_id = (uint32_t) MASTER_COMMANDS_MOTOR;
                       //send our message
                       if(iCAN_tx(&msg_tx, (uint16_t) MASTER_COMMANDS_MOTOR))   {
                          printf("Message sent to motor!\n");
                          LE.toggle(3);
                       }

                       //reset our state
                       g_current_state = STATE_MACHINE_END;

                       break;

                   default:
                       g_current_state = STATE_MACHINE_END;

                       break;


               } //end switch(g_current_state)

           }//end while
        ///////////////////////////////////////////////////////////////



    delete temp_rx;
    //delete default_sensor_msg;


} //end 100Hz

void period_1000Hz(void)
{

    //LE.toggle(4);


} //end 1000Hz

void handleMessage()
{

}


