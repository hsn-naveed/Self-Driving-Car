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

/*
 *
 *
 *          MASTER CONTROLLER TEAM NOTES!!!
 *
 *         12/01 - Marvin's update
 *              - changed all state cases to enum, removing a lot of arbitrary values
 *              - generated can_message.c from db parser
 *              - changed the code so that we won't be needing the can_storage.cpp,
 *                  everything will be inside the periodic_callbacks.cpp
 *                  I think this is necessary since conflicts happen when we include can_message.c
 *                  in our can_storage.hpp, thus we could not instantiate our structs inside
 *                  our can_storage. This also removes the overhead that we have every time we
 *                  need to access our data. Having global variables is easier and more readable.
 *
 *              TO DO:
 *                  - fix errors regarding sending and receiving CAN messages. Everything was changed due
 *                     to implementation of dbc parser.
 *                  - remove can_storage totally, we won't be needing this once we have the
 *                      necessary global variables.
 *                  - Test the logic for sensor and motor again. Since most things were changed, we need a
 *                      couple of runs to make sure we didn't mess up the logic
 *                  - Add implementation for the headings coming from the GPS team
 *
 *
 *
 *          12/02 - Marvin's update
 *                  - fixed rx and tx operation - UNTESTED
 *                  - tested the sensor and motor logic using switches - RESULT OK!
 *                  - removed the switch statement in our logic; our code now runs straight through,
 *                      sending motor commands at the end of 100Hz task
 *                  - reverse command still sends STOP signal
 *                  - implemented sonar message MIA
 *
 *              TO DO:
 *                  - test CAN rx and tx
 *                  - implement heading logic
 *                  - implement reverse (the system should only do reverse + hard_left for simplicity)
 *
 *
 *
 */

#include <stdio.h>
#include <stdint.h>
#include "io.hpp"
#include "periodic_callback.h"
#include "file_logger.h"
#include "shared_handles.h"
#include "tasks.hpp"
#include <inttypes.h>
#include <math.h>
#include "can.h"

// Include file for telemetry --> ALso need to turn on #define at sys_config.h (SYS_CFG_ENABLE_TLM)
#include "tlm/c_tlm_comp.h"
#include "tlm/c_tlm_var.h"


#include "243_can/CAN_structs.h"
#include "243_can/iCAN.hpp"

#include "can_message.h"

/// This is the stack size used for each of the period tasks
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

// set to 1 if you want to test motor controls using switches
// set to 0 if you want to use CAN (normal mode)
#define DEBUG_NO_CAN 1


///////////////////////////LOGIC///////////////////////////

//States
enum MainStateMachine {
    CHECK_SENSOR_VALUES,
    CONTROL_BASED_ON_SENSOR,
    CONTROL_BASED_ON_HEADING,
    SEND_CONTROL_TO_MOTOR,
    STATE_MACHINE_END,
};

//current state
MainStateMachine g_current_state = CHECK_SENSOR_VALUES;

//previous state
//volatile int g_prev_state = 0;
MainStateMachine g_prev_state = g_current_state;

enum VehicleMode {
    FREERUN,
    AUTO,
};

//we initialize to freerun mode
VehicleMode g_current_mode =  FREERUN;

///////////////////////////----///////////////////////////


///////////////////////////MOTOR///////////////////////////
//Commands for motor
enum MotorCommands {
    COMMAND_MOTOR_STOP,
    COMMAND_MOTOR_FORWARD_STRAIGHT_SLOW,
    COMMAND_MOTOR_FORWARD_STRAIGHT_MEDIUM,
    COMMAND_MOTOR_FORWARD_STRAIGHT_FAST,
    COMMAND_MOTOR_FORWARD_SOFT_LEFT,
    COMMAND_MOTOR_FORWARD_HARD_LEFT,
    COMMAND_MOTOR_FORWARD_SOFT_RIGHT,
    COMMAND_MOTOR_FORWARD_HARD_RIGHT,
    COMMAND_MOTOR_REVERSE_STRAIGHT,
    COMMAND_MOTOR_REVERSE_LEFT,
    COMMAND_MOTOR_REVERSE_RIGHT,
};

MASTER_TX_MOTOR_CMD_t* g_motor_cmd_values;

uint8_t g_motor_cmd_steer_value = 0;
uint8_t g_motor_cmd_drive_value = 0;

void generateMotorCommands(MotorCommands command);
///////////////////////////----///////////////////////////


///////////////////////////SENSOR///////////////////////////

//for left and right sensors
static uint8_t MINIMUM_LR_SENSOR_VALUE = 50;

//for middle sensor
static uint8_t MINIMUM_MIDDLE_SENSOR_VALUE = 30;

//for rear sensor
static uint8_t MINIMUM_REAR_SENSOR_VALUE = 16;

//for all front sensor
//unused
static uint8_t CRITICAL_SENSOR_VALUE = 8;

//Sensor readings based on minimum values
bool LEFT_BLOCKED = false;
bool RIGHT_BLOCKED = false;
bool MIDDLE_BLOCKED = false;
bool REAR_BLOCKED = false;

//if all three are clear
bool FRONT_CLEAR = false;

//sensor values
uint8_t g_sensor_left_value = 0;
uint8_t g_sensor_middle_value = 0;
uint8_t g_sensor_right_value = 0;
uint8_t g_sensor_rear_value = 0;

SENSOR_TX_INFO_SONARS_t* g_sensor_values;

// EXACT NAME: MIA replacement
const SENSOR_TX_INFO_SONARS_t INFO_SONARS__MIA_MSG = { 5, 5, 5, 5};

// EXACT NAME: Timeout when MIA is replaced
const uint32_t INFO_SONARS__MIA_MS = 1000;

///////////////////////////----///////////////////////////


///////////////////////////GPS///////////////////////////
GPS_TX_INFO_HEADING_t* g_heading_values;
GPS_TX_DESTINATION_REACHED_t* g_dest_reached_value;

double g_heading_current_value = 0;
double g_heading_destination_value = 0;

///////////////////////////----///////////////////////////


///////////////////////////ANDROID///////////////////////////
bool g_receiving_checkpoints = false;

bool g_GO_signal = false;

ANDROID_TX_STOP_GO_CMD_t* g_android_stop_go_value;
///////////////////////////----///////////////////////////


///////////////////////////COUNTERS///////////////////////////
//counter for the receive sensor
int g_sensor_receive_counter = 0;

//counter for the GPS
int g_gps_receive_counter = 0;

//counter for the Compass
int g_compass_receive_counter = 0;

const int g_reset = 0;
const int g_max_count_timer = 300; // we're running in 100Hz and we expect messages within 3Hz.

//use as heart beat monitor counter
double g_heart_counter = 0;
///////////////////////////----///////////////////////////


///////////////////////////DISPLAY///////////////////////////
//for 7LED display
enum Display7LED {
    DISPLAY_FORWARD_STRAIGHT,
    DISPLAY_FORWARD_SOFT_LEFT,
    DISPLAY_FORWARD_HARD_LEFT,
    DISPLAY_FORWARD_SOFT_RIGHT,
    DISPLAY_FORWARD_HARD_RIGHT,
    DISPLAY_STOP,
    DISPLAY_REVERSE_STRAIGHT,
    DISPLAY_REVERSE_LEFT,
    DISPLAY_REVERSE_RIGHT,
    GPS_LOST,
    COMPASS_LOST,
    DISPLAY_ERROR,
};
///////////////////////////----///////////////////////////


///////////////////////////CAN MESSAGES///////////////////////////
can_fullcan_msg_t *g_sensor_msg;
can_fullcan_msg_t *g_gps_msg;
can_fullcan_msg_t *g_compass_msg;
can_fullcan_msg_t *g_android_msg;

can_fullcan_msg_t* g_receive_msg;

//used for sending data
can_msg_t* msg_tx;

//used for message ids
msg_hdr_t master_heartbeat_hdr = MASTER_TX_HEARTBEAT_HDR;
msg_hdr_t sensor_info_sonar_hdr = SENSOR_TX_INFO_SONARS_HDR;
msg_hdr_t master_motor_cmd_hdr = MASTER_TX_MOTOR_CMD_HDR;
msg_hdr_t android_stop_go_cmd_hdr = ANDROID_TX_STOP_GO_CMD_HDR;
msg_hdr_t android_info_checkpoints_hdr = ANDROID_TX_INFO_CHECKPOINTS_HDR;
msg_hdr_t android_info_coordinates_hdr = ANDROID_TX_INFO_COORDINATES_HDR;
msg_hdr_t gps_info_heading_hdr = GPS_TX_INFO_HEADING_HDR ;
msg_hdr_t gps_destination_reached_hdr = GPS_TX_DESTINATION_REACHED_HDR;

///////////////////////////----///////////////////////////




/// Called once before the RTOS is started, this is a good place to initialize things once
bool period_init(void)
{
    //intialize our messages here
    g_gps_msg = new can_fullcan_msg_t { 0 };
    g_compass_msg = new can_fullcan_msg_t { 0 };
    g_sensor_msg = new can_fullcan_msg_t { 0 };
    g_android_msg = new can_fullcan_msg_t { 0 };

    msg_tx = new can_msg_t {0};

    g_motor_cmd_values = new MASTER_TX_MOTOR_CMD_t {0};
    g_sensor_values = new SENSOR_TX_INFO_SONARS_t {0};
    g_heading_values = new GPS_TX_INFO_HEADING_t {0};
    g_android_stop_go_value = new ANDROID_TX_STOP_GO_CMD_t {0};
    g_dest_reached_value = new GPS_TX_DESTINATION_REACHED_t {0};

    return true; // Must return true upon success

}

/// Register any telemetry variables
bool period_reg_tlm(void)
{
    // Make sure "SYS_CFG_ENABLE_TLM" is enabled at sys_config.h to use Telemetry

    tlm_component *master_cmp = tlm_component_add("master");

    TLM_REG_VAR(master_cmp, g_heart_counter, tlm_double);

    //blocked sensor values
    // Add the variable under "disk" variable
   TLM_REG_VAR(tlm_component_get_by_name("disk"), MINIMUM_LR_SENSOR_VALUE, tlm_uint);
   TLM_REG_VAR(tlm_component_get_by_name("disk"), MINIMUM_MIDDLE_SENSOR_VALUE, tlm_uint);

    //sensor values
    TLM_REG_VAR(master_cmp,  g_sensor_left_value, tlm_uint);
    TLM_REG_VAR(master_cmp,  g_sensor_middle_value, tlm_uint);
    TLM_REG_VAR(master_cmp,  g_sensor_right_value, tlm_uint);
    TLM_REG_VAR(master_cmp,  g_sensor_rear_value, tlm_uint);

    //motor commands
    TLM_REG_VAR(master_cmp,g_motor_cmd_steer_value, tlm_uint);
    TLM_REG_VAR(master_cmp, g_motor_cmd_drive_value, tlm_uint);

    //heading values
    TLM_REG_VAR(master_cmp,g_heading_current_value, tlm_double);
       TLM_REG_VAR(master_cmp, g_heading_destination_value, tlm_double);

    return true; // Must return true upon success
}

void setDirectionBasedHeading() {
        float tolerance = 20.0;
        float current_sector = round(g_heading_values->GPS_INFO_HEADING_current/tolerance);
        float dst_sector = round(g_heading_values->GPS_INFO_HEADING_dst/tolerance);
    
        //if we are in +/- 20 degrees of our destination heading, maintain course
        if ((int)current_sector == (int)dst_sector)//+- X%
            {
            //go straight
            generateMotorCommands(COMMAND_MOTOR_FORWARD_STRAIGHT_SLOW);
        }
        else if(g_heading_values->GPS_INFO_HEADING_dst>g_heading_values->GPS_INFO_HEADING_current) {
            if (abs(g_heading_values->GPS_INFO_HEADING_dst - g_heading_values->GPS_INFO_HEADING_current)>=180){
                generateMotorCommands(COMMAND_MOTOR_FORWARD_SOFT_RIGHT);
            }
            else{
                generateMotorCommands(COMMAND_MOTOR_FORWARD_SOFT_LEFT);
            }
        }
        else{
            if (abs(g_heading_values->GPS_INFO_HEADING_dst - g_heading_values->GPS_INFO_HEADING_current)>=180){
                generateMotorCommands(COMMAND_MOTOR_FORWARD_SOFT_LEFT);
            }
            else{
                generateMotorCommands(COMMAND_MOTOR_FORWARD_SOFT_RIGHT);
            }
        }

}

void disp_7LED(Display7LED val)
{
    switch (val)
    {
        case DISPLAY_FORWARD_STRAIGHT:
            LD.clear();
            LD.setLeftDigit('1');
            LD.setRightDigit('1');
            break;

        case DISPLAY_FORWARD_HARD_LEFT:
            LD.clear();
            LD.setLeftDigit('H');
            LD.setRightDigit('L');
            break;

        case DISPLAY_FORWARD_SOFT_LEFT:
            LD.clear();
            LD.setLeftDigit('S');
            LD.setRightDigit('L');
            break;

        case DISPLAY_FORWARD_HARD_RIGHT:
            LD.clear();
            LD.setLeftDigit('H');
            LD.setRightDigit('9');
            break;

        case DISPLAY_FORWARD_SOFT_RIGHT:
            LD.clear();
            LD.setLeftDigit('S');
            LD.setRightDigit('9');
            break;

        case DISPLAY_STOP:
            LD.clear();
            LD.setLeftDigit('S');
            LD.setRightDigit('S');
            break;

        case DISPLAY_REVERSE_STRAIGHT:
            LD.clear();
            LD.setLeftDigit('6');
            LD.setRightDigit('9');
            break;

        case DISPLAY_REVERSE_LEFT:
            LD.clear();
            LD.setLeftDigit('6');
            LD.setRightDigit('9');
            break;

        case DISPLAY_REVERSE_RIGHT:
            LD.clear();
            LD.setLeftDigit('6');
            LD.setRightDigit('9');
            break;

        case DISPLAY_ERROR:
            LD.clear();
            LD.setLeftDigit('E');
            LD.setRightDigit('E');
            break;
        case GPS_LOST:
            LD.clear();
            LD.setLeftDigit('G');
            LD.setRightDigit('L');
            break;
        case COMPASS_LOST:
            LD.clear();
            LD.setLeftDigit('C');
            LD.setRightDigit('L');
            break;

        default:
            LD.clear();
            LD.setLeftDigit('8');
            LD.setRightDigit('8');
            break;
        }

    } //end disp_LED

//this function is not used anymore because of telemetry
void printMotorCommand(MotorCommands print_command)
{

    switch (print_command)
    {
        case COMMAND_MOTOR_STOP:
            // printf("COMMAND_MOTOR_STOP\n");
            break;

        case COMMAND_MOTOR_FORWARD_STRAIGHT_SLOW:
            //printf("COMMAND_MOTOR_FORWARD_STRAIGHT\n");
            break;

        case COMMAND_MOTOR_FORWARD_HARD_LEFT:
            // printf("COMMAND_MOTOR_FORWARD_LEFT\n");
            break;

        case COMMAND_MOTOR_FORWARD_HARD_RIGHT:
            // printf("COMMAND_MOTOR_FORWARD_RIGHT\n");
            break;

        case COMMAND_MOTOR_REVERSE_STRAIGHT:
             printf("COMMAND_MOTOR_REVERSE_STRAIGHT\n");
            break;

        default:
            // printf("COMMAND_MOTOR_REVERSE_RIGHT\n");
            break;
    }
}

void parseSensorReading(SENSOR_TX_INFO_SONARS_t* data)
{

    if (data->SENSOR_INFO_SONARS_left < MINIMUM_LR_SENSOR_VALUE)
        LEFT_BLOCKED = true;
    else
        LEFT_BLOCKED = false;

    if (data->SENSOR_INFO_SONARS_middle < MINIMUM_MIDDLE_SENSOR_VALUE)
        MIDDLE_BLOCKED = true;
    else
        MIDDLE_BLOCKED = false;

    if (data->SENSOR_INFO_SONARS_right < MINIMUM_LR_SENSOR_VALUE)
        RIGHT_BLOCKED = true;
    else
        RIGHT_BLOCKED = false;

    if (data->SENSOR_INFO_SONARS_rear < MINIMUM_REAR_SENSOR_VALUE)
        REAR_BLOCKED = true;
    else
        REAR_BLOCKED = false;

    if (!LEFT_BLOCKED && !MIDDLE_BLOCKED && !RIGHT_BLOCKED)
        FRONT_CLEAR = true;
    else
        FRONT_CLEAR = false;

}


void generateMotorCommands(MotorCommands command)
{
    switch (command)
        {
            //All these cases should be enum values
            case COMMAND_MOTOR_STOP:
                g_motor_cmd_values->MASTER_MOTOR_CMD_steer = (uint8_t) COMMAND_STRAIGHT;
                g_motor_cmd_values->MASTER_MOTOR_CMD_drive = (uint8_t) COMMAND_STOP;
                disp_7LED(DISPLAY_STOP);
                break;

            case COMMAND_MOTOR_FORWARD_STRAIGHT_SLOW:
                g_motor_cmd_values->MASTER_MOTOR_CMD_steer = (uint8_t) COMMAND_STRAIGHT;

                g_motor_cmd_values->MASTER_MOTOR_CMD_drive = (uint8_t) COMMAND_SLOW;
                disp_7LED(DISPLAY_FORWARD_STRAIGHT);
                break;

            case COMMAND_MOTOR_FORWARD_STRAIGHT_MEDIUM:
                g_motor_cmd_values->MASTER_MOTOR_CMD_steer = (uint8_t) COMMAND_STRAIGHT;
                g_motor_cmd_values->MASTER_MOTOR_CMD_drive = (uint8_t) COMMAND_MEDIUM;
                disp_7LED(DISPLAY_FORWARD_STRAIGHT);
                break;

            case COMMAND_MOTOR_FORWARD_STRAIGHT_FAST:
                g_motor_cmd_values->MASTER_MOTOR_CMD_steer = (uint8_t) COMMAND_STRAIGHT;
                g_motor_cmd_values->MASTER_MOTOR_CMD_drive = (uint8_t) COMMAND_FAST;
                disp_7LED(DISPLAY_FORWARD_STRAIGHT);
                break;

            case COMMAND_MOTOR_FORWARD_HARD_LEFT:
                g_motor_cmd_values->MASTER_MOTOR_CMD_steer = (uint8_t) COMMAND_HARD_LEFT;
                g_motor_cmd_values->MASTER_MOTOR_CMD_drive = (uint8_t) COMMAND_SLOW;
                disp_7LED(DISPLAY_FORWARD_HARD_LEFT);
                break;

            case COMMAND_MOTOR_FORWARD_HARD_RIGHT:
                g_motor_cmd_values->MASTER_MOTOR_CMD_steer = (uint8_t) COMMAND_HARD_RIGHT;
                g_motor_cmd_values->MASTER_MOTOR_CMD_drive = (uint8_t) COMMAND_SLOW;
                disp_7LED(DISPLAY_FORWARD_HARD_RIGHT);
                break;

            case COMMAND_MOTOR_REVERSE_LEFT:
                g_motor_cmd_values->MASTER_MOTOR_CMD_steer = (uint8_t) COMMAND_HARD_LEFT;
                g_motor_cmd_values->MASTER_MOTOR_CMD_drive = (uint8_t) COMMAND_REVERSE;//change this for reverse!
                disp_7LED(DISPLAY_REVERSE_LEFT);
                break;

            case COMMAND_MOTOR_REVERSE_RIGHT:
                g_motor_cmd_values->MASTER_MOTOR_CMD_steer = (uint8_t) COMMAND_HARD_RIGHT;
                g_motor_cmd_values->MASTER_MOTOR_CMD_drive = (uint8_t) COMMAND_REVERSE;//change this for reverse!
                disp_7LED(DISPLAY_REVERSE_RIGHT);
                break;

            case COMMAND_MOTOR_FORWARD_SOFT_LEFT:
                g_motor_cmd_values->MASTER_MOTOR_CMD_steer = (uint8_t) COMMAND_SOFT_LEFT;
                g_motor_cmd_values->MASTER_MOTOR_CMD_drive = (uint8_t) COMMAND_SLOW;
                disp_7LED(DISPLAY_FORWARD_SOFT_LEFT);

                break;

            case COMMAND_MOTOR_FORWARD_SOFT_RIGHT:
                g_motor_cmd_values->MASTER_MOTOR_CMD_steer = (uint8_t) COMMAND_SOFT_RIGHT;
                g_motor_cmd_values->MASTER_MOTOR_CMD_drive = (uint8_t) COMMAND_SLOW;
                disp_7LED(DISPLAY_FORWARD_SOFT_RIGHT);
                break;

            default:
                g_motor_cmd_values->MASTER_MOTOR_CMD_steer = (uint8_t) COMMAND_STRAIGHT;
                g_motor_cmd_values->MASTER_MOTOR_CMD_drive = (uint8_t) COMMAND_STOP;
                disp_7LED(DISPLAY_STOP);
                break;
        }
    //printMotorCommand(command);
}


void period_1Hz(void)
{
    LE.toggle(1);
    //printf("\n\n\nHEART BEAT: %d\n\n\n", g_heart_counter++);
    g_heart_counter++;
    if (50000 < g_heart_counter) g_heart_counter = 0;

}

void period_10Hz(void)
{

    // delete l_sensor_values;

} // end 10Hz task

void period_100Hz(void)
{

    /*
     * RECEIVE AND SAVE FULL_CAN MESSAGES
     *
     */
    //SENSOR_TX_INFO_SONARS_decode(SENSOR_TX_INFO_SONARS_t *to, const uint64_t *from, const msg_hdr_t *hdr)
//    msg_hdr_t receive_hdr = {0,0};
    //iCAN_rx(g_receive_msg, &sensor_info_sonar_hdr);
//    switch ((uint32_t)receive_hdr.mid) {
//            case 700:
//            {
//                SENSOR_TX_INFO_SONARS_decode(g_sensor_values, (uint64_t*) g_receive_msg->data.qword, &SENSOR_TX_INFO_SONARS_HDR);
//                break;
//            }
//        }



    if (iCAN_rx(g_sensor_msg, &sensor_info_sonar_hdr))
    {

        portDISABLE_INTERRUPTS();
        SENSOR_TX_INFO_SONARS_decode(g_sensor_values, (uint64_t*) &g_sensor_msg->data.qword, &SENSOR_TX_INFO_SONARS_HDR);
        portENABLE_INTERRUPTS();

        g_sensor_receive_counter = g_reset;
    }
    //Compass sending current heading
    //TO DO @hsn_naveed
    else if (iCAN_rx(g_compass_msg, &gps_info_heading_hdr))
    {
        portDISABLE_INTERRUPTS();
        GPS_TX_INFO_HEADING_decode(g_heading_values,(uint64_t*) &(g_compass_msg->data.qword), &GPS_TX_INFO_HEADING_HDR);
        portENABLE_INTERRUPTS();

        printf("HEADING current %" PRIu32 "\n", (uint32_t) g_heading_values->GPS_INFO_HEADING_current);
        printf("HEADING dest %" PRIu32 "\n", (uint32_t) g_heading_values->GPS_INFO_HEADING_dst);

        g_heading_current_value = (double) g_heading_values->GPS_INFO_HEADING_current;
        g_heading_destination_value = (double) g_heading_values->GPS_INFO_HEADING_dst;

        g_compass_receive_counter = g_reset;

    }
    //Parse STOP&GO signal message
    else if (iCAN_rx(g_android_msg, &android_stop_go_cmd_hdr))
    {
        portDISABLE_INTERRUPTS();
        ANDROID_TX_STOP_GO_CMD_decode(g_android_stop_go_value, (uint64_t*) &g_android_msg->data.qword, &ANDROID_TX_STOP_GO_CMD_HDR);
        portENABLE_INTERRUPTS();

        if((uint8_t) g_android_stop_go_value->ANDROID_STOP_CMD_signal == 1) {
            g_GO_signal = true;
        } else g_GO_signal = false;
    }



#if !DEBUG_NO_CAN
//    if (g_sensor_receive_counter > g_max_count_timer)
//    {
//
//        portDISABLE_INTERRUPTS();
//        CAN_ST.setSafeSensorValues();
//        portENABLE_INTERRUPTS();
//
//        //reset our counter because we fill-in safe values
//        g_sensor_receive_counter = g_reset;
//    }
#endif



    ///////////////////////////////////////////////////////////////

#if DEBUG_NO_CAN == 1

    int sw_value = SW.getSwitchValues();
    switch(sw_value)
    {
        case 0:
        portDISABLE_INTERRUPTS();
        g_sensor_msg->data.bytes[0] = (uint8_t) 0xff;
        g_sensor_msg->data.bytes[1] = (uint8_t) 0xff;
        g_sensor_msg->data.bytes[2] = (uint8_t) 0xff;
        g_sensor_msg->data.bytes[3] = (uint8_t) 0xff;
        CAN_ST.sensor_data = (sen_msg_t*) & g_sensor_msg->data.bytes[0];
        portENABLE_INTERRUPTS();
        break;

        case 1:
        portDISABLE_INTERRUPTS();
        g_sensor_msg->data.bytes[0] = (uint8_t) 0x00;
        g_sensor_msg->data.bytes[1] = (uint8_t) 0xff;
        g_sensor_msg->data.bytes[2] = (uint8_t) 0xff;
        g_sensor_msg->data.bytes[3] = (uint8_t) 0xff;
        CAN_ST.sensor_data = (sen_msg_t*) & g_sensor_msg->data.bytes[0];
        portENABLE_INTERRUPTS();
        break;

        case 2:
        portDISABLE_INTERRUPTS();
        g_sensor_msg->data.bytes[0] = (uint8_t) 0xff;
        g_sensor_msg->data.bytes[1] = (uint8_t) 0x00;
        g_sensor_msg->data.bytes[2] = (uint8_t) 0xff;
        g_sensor_msg->data.bytes[3] = (uint8_t) 0xff;
        CAN_ST.sensor_data = (sen_msg_t*) & g_sensor_msg->data.bytes[0];
        portENABLE_INTERRUPTS();
        break;

        case 3:
        portDISABLE_INTERRUPTS();
        g_sensor_msg->data.bytes[0] = (uint8_t) 0x00;
        g_sensor_msg->data.bytes[1] = (uint8_t) 0x00;
        g_sensor_msg->data.bytes[2] = (uint8_t) 0xff;
        g_sensor_msg->data.bytes[3] = (uint8_t) 0xff;

        portENABLE_INTERRUPTS();
        break;

        case 4:
        portDISABLE_INTERRUPTS();
        g_sensor_msg->data.bytes[0] = (uint8_t) 0xff;
        g_sensor_msg->data.bytes[1] = (uint8_t) 0xff;
        g_sensor_msg->data.bytes[2] = (uint8_t) 0x00;
        g_sensor_msg->data.bytes[3] = (uint8_t) 0xff;

        portENABLE_INTERRUPTS();
        break;

        case 5:
        portDISABLE_INTERRUPTS();
        g_sensor_msg->data.bytes[0] = (uint8_t) 0x00;
        g_sensor_msg->data.bytes[1] = (uint8_t) 0xff;
        g_sensor_msg->data.bytes[2] = (uint8_t) 0x00;
        g_sensor_msg->data.bytes[3] = (uint8_t) 0xff;

        portENABLE_INTERRUPTS();
        break;

        case 6:
        portDISABLE_INTERRUPTS();
        g_sensor_msg->data.bytes[0] = (uint8_t) 0xff;
        g_sensor_msg->data.bytes[1] = (uint8_t) 0x00;
        g_sensor_msg->data.bytes[2] = (uint8_t) 0x00;
        g_sensor_msg->data.bytes[3] = (uint8_t) 0xff;

        portENABLE_INTERRUPTS();
        break;

        case 7:
        portDISABLE_INTERRUPTS();
        g_sensor_msg->data.bytes[0] = (uint8_t) 0x00;
        g_sensor_msg->data.bytes[1] = (uint8_t) 0x00;
        g_sensor_msg->data.bytes[2] = (uint8_t) 0x00;
        g_sensor_msg->data.bytes[3] = (uint8_t) 0xff;

        portENABLE_INTERRUPTS();
        break;

        case 8:
        portDISABLE_INTERRUPTS();
        g_sensor_msg->data.bytes[0] = (uint8_t) 0xff;
        g_sensor_msg->data.bytes[1] = (uint8_t) 0xff;
        g_sensor_msg->data.bytes[2] = (uint8_t) 0xff;
        g_sensor_msg->data.bytes[3] = (uint8_t) 0x00;

        portENABLE_INTERRUPTS();
        break;

        case 15:
        portDISABLE_INTERRUPTS();
        g_sensor_msg->data.bytes[0] = (uint8_t) 0x00;
        g_sensor_msg->data.bytes[1] = (uint8_t) 0x00;
        g_sensor_msg->data.bytes[2] = (uint8_t) 0x00;
        g_sensor_msg->data.bytes[3] = (uint8_t) 0x00;

        portENABLE_INTERRUPTS();
        break;

        default:
        portDISABLE_INTERRUPTS();
        g_sensor_msg->data.bytes[0] = (uint8_t) 0x00;
        g_sensor_msg->data.bytes[1] = (uint8_t) 0x00;
        g_sensor_msg->data.bytes[2] = (uint8_t) 0x00;
        g_sensor_msg->data.bytes[3] = (uint8_t) 0x00;

        portENABLE_INTERRUPTS();
        break;
    }

    //for heading simulation
//    g_compass_msg->data.dwords[0] = (uint32_t) abs(AS.getX())/3;
//    g_compass_msg->data.dwords[1] = (uint32_t) abs(AS.getY())/3;
    //printf("x: %" PRIu32 "\n" ,(uint32_t) g_compass_msg->data.dwords[0] );
#endif

    //NO MESSAGE RECEIVE LOGIC
    //>>>>>>>>>>>>>>>>>>>>>HANDLE MIA
       if (SENSOR_TX_INFO_SONARS_handle_mia(g_sensor_values, 30)) {
           //SENSOR MISS
       }

#if !DEBUG_NO_CAN
    //setting modes
    int sw_value = SW.getSwitchValues();
    if (sw_value != 0)  {
        switch (sw_value)
        {
            case 1:
                g_current_mode =  AUTO;
                LE.setAll(0);
                LE.set(3, true);
                break;
            case 2:
                g_current_mode =  AUTO;
                LE.setAll(0);
                LE.set(3, true);
                break;
            case 4:
                g_current_mode =  AUTO;
                LE.setAll(0);
                LE.set(3, true);
                break;

            case 8:
                g_current_mode = FREERUN;
                LE.setAll(0);
                LE.set(4, true);
                break;

                default:

                break;
        }
    }


#endif

   // g_heading_values = (GPS_TX_INFO_HEADING_t*) &g_compass_msg->data.dwords[0];

  //  printf("%" PRIu32 " " ,(uint32_t) g_heading_values->GPS_INFO_HEADING_current );
   // printf("%" PRIu32 " \n" ,(uint32_t) g_heading_values->GPS_INFO_HEADING_dst );

    g_sensor_values =(SENSOR_TX_INFO_SONARS_t*) & g_sensor_msg->data.bytes[0];
    //prepare our motor commands container
    g_motor_cmd_values = (MASTER_TX_MOTOR_CMD_t*) & msg_tx->data.bytes[0];

    // we send STOP command while we wait for GO signal from Android
    if (!g_GO_signal && (g_current_mode ==  AUTO))
    {
       //generate command to STOP
        generateMotorCommands(COMMAND_MOTOR_STOP);

    }
    //if we're in FREERUN or we receive a GO signal
    else if ( (g_current_mode == FREERUN) || (g_GO_signal && g_current_mode ==  AUTO) )
    {

        //>>>>>>>>>>>>>>>>>READING SENSOR
        //First State is to check our sensor values
        //we parse the sensor values
        parseSensorReading(g_sensor_values);

        g_sensor_left_value = (uint8_t) g_sensor_values->SENSOR_INFO_SONARS_left;
        g_sensor_middle_value = (uint8_t) g_sensor_values->SENSOR_INFO_SONARS_middle;
        g_sensor_right_value = (uint8_t) g_sensor_values->SENSOR_INFO_SONARS_right;
        g_sensor_rear_value = (uint8_t) g_sensor_values->SENSOR_INFO_SONARS_rear;

        //>>>>>>>>>>>>>>>>>GENERATE DRIVE COMMANDS
        //drive motor based on HEADING
        if(FRONT_CLEAR) {
            //this is where we command based on the heading
            //TO DO: GPS IMPLEMENTATION

            // now we're testing without GPS
            // so we go straight
            generateMotorCommands(COMMAND_MOTOR_FORWARD_STRAIGHT_MEDIUM);
        }
        //drive motor based on sensor
        else {
            // only left is blocked         !l m r = -->
            if (LEFT_BLOCKED && !RIGHT_BLOCKED && !MIDDLE_BLOCKED)
            {
                generateMotorCommands(COMMAND_MOTOR_FORWARD_SOFT_RIGHT); //turn soft right

            }
            // only right is blocked
            else if(RIGHT_BLOCKED && !LEFT_BLOCKED && !MIDDLE_BLOCKED)
            {
                generateMotorCommands(COMMAND_MOTOR_FORWARD_SOFT_LEFT); //turn soft left

            }
            //only middle is blocked
            else if(MIDDLE_BLOCKED && !LEFT_BLOCKED && !RIGHT_BLOCKED)
            {
                //turn based on left and right sensor values
                if(g_sensor_left_value > g_sensor_right_value) {
                    generateMotorCommands(COMMAND_MOTOR_FORWARD_SOFT_LEFT); //turn left
                } else {
                    generateMotorCommands(COMMAND_MOTOR_FORWARD_SOFT_RIGHT); //turn right
                }

            }
            //if left and middle is blocked
            else if (LEFT_BLOCKED && MIDDLE_BLOCKED && !RIGHT_BLOCKED)
            {

                generateMotorCommands(COMMAND_MOTOR_FORWARD_HARD_RIGHT); //turn FULL right

            }
            //if right and middle is blocked
            else if (RIGHT_BLOCKED && MIDDLE_BLOCKED && !LEFT_BLOCKED)
            {
                generateMotorCommands(COMMAND_MOTOR_FORWARD_HARD_LEFT); //turn FULL left
                //generateMotorCommands(COMMAND_MOTOR_STOP);

            }
            //if left and right are blocked
            else if(LEFT_BLOCKED && RIGHT_BLOCKED && !MIDDLE_BLOCKED)
            {
                generateMotorCommands(COMMAND_MOTOR_FORWARD_STRAIGHT_SLOW); //keep going straight
                //generateMotorCommands(COMMAND_MOTOR_STOP);

            }
            //if all front sensors are blocked
            else if (!FRONT_CLEAR)
            {
                //if back sensor is also blocked
                if (REAR_BLOCKED)
                {
                   generateMotorCommands(COMMAND_MOTOR_STOP);
                    // generateMotorCommands(COMMAND_MOTOR_REVERSE_LEFT);
                    disp_7LED(DISPLAY_STOP);
                }
                //if back is clear, reverse-left
                else
                {
                    generateMotorCommands(COMMAND_MOTOR_REVERSE_LEFT);

                }
            }
            else
            {
                generateMotorCommands(COMMAND_MOTOR_STOP);

            }
        }

        ///////////////////////////SENDING PART///////////////////////////////

        //copy our values for telemetry

        g_motor_cmd_steer_value = (uint8_t) g_motor_cmd_values->MASTER_MOTOR_CMD_steer;
        g_motor_cmd_drive_value = (uint8_t) g_motor_cmd_values->MASTER_MOTOR_CMD_drive;
        //>>>>>>>>>>>>>>>>>SEND COMM1ANDS TO THE MOTOR
        //prepare our message id
        msg_tx->msg_id = (uint32_t) MASTER_TX_MOTOR_CMD_HDR.mid;

        msg_hdr_t encoded_message = MASTER_TX_MOTOR_CMD_encode((uint64_t*)&(msg_tx->data.qword), g_motor_cmd_values);
        //send our message
        if(iCAN_tx(msg_tx, &encoded_message))
        {
            // printf("Message sent to motor\n");
            // printf("Steer value: %i\n",msg_tx->data.bytes[0]);
            // printf("drive value: %i\n",msg_tx->data.bytes[1]);
            //printf("byte 0 = %i\n",g_motor_cmd_values->MASTER_MOTOR_CMD_steer);
            //printf("byte 1 = %i\n",(uint8_t) g_motor_cmd_values->MASTER_MOTOR_CMD_drive);

        } else {
            //printf("\nNo message sent\n");
        }


    } //else if ( (g_current_mode == FREERUN) || (g_GO_signal && g_current_mode ==  AUTO) )

    ///////////////////////////////////////////////////////////////


} //end 100Hz

void period_1000Hz(void)
{

    //LE.toggle(4);

} //end 1000Hz

void handleMessage()
{

}

