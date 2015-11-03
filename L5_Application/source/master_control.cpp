/*
 * master_control.cpp
 *
 *  Created on: Oct 24, 2015
 *      Author: Marvin
 */


#include <stdio.h>

#include "master_control.hpp"
#include "rtc_alarm.h"
#include "rtc.h"
#include "file_logger.h"
#include "io.hpp"
#include "c_tlm_var.h"
#include "io.hpp"
#include "can.h"
#include <inttypes.h>


#include "L4_IO/can_definitions.hpp"
#include "L4_IO/can_storage.hpp"

#include "243_can/CAN_structs.h"


control_handler_task::control_handler_task(uint8_t priority) :
   scheduler_task("control_handler_task", 8*512, priority),
   mCANMessage_QueueHandler(NULL),
   mCANMessage_QueueHandler_FromTerminal(NULL),
   mCANMessage_QueueHandler_Receive(NULL),
   mCANMessage_QueueHandler_Transmit(NULL) {

    mCAN_MSG_Rx = { 0 };
    mCAN_MSG_Tx = { 0 };

}

bool control_handler_task::init() {

    //coming from the period_callback functions
    mCANMessage_QueueHandler = xQueueCreate(1, sizeof(can_msg_t));
    addSharedObject(shared_CAN_message_queue_master, mCANMessage_QueueHandler);
    mCANMessage_QueueHandler = getSharedObject(shared_CAN_message_queue_master);

    //we need this for the real message coming from the CAN message handler task
    mCANMessage_QueueHandler_Receive = xQueueCreate(1, sizeof(can_msg_t));
    addSharedObject(shared_CAN_message_queue_receive, mCANMessage_QueueHandler_Receive);
    mCANMessage_QueueHandler_Receive = getSharedObject(shared_CAN_message_queue_receive);

    //
    mCANMessage_QueueHandler_Transmit = xQueueCreate(1, sizeof(can_msg_t));
    addSharedObject(shared_CAN_message_queue_transmit, mCANMessage_QueueHandler_Transmit);
    mCANMessage_QueueHandler_Transmit = getSharedObject(shared_CAN_message_queue_transmit);


    //we need this for the terminal, simulates the receive side of the CAN bus
    mCANMessage_QueueHandler_FromTerminal = xQueueCreate(1, sizeof(can_msg_t));
    addSharedObject(shared_CAN_message_queue_terminal, mCANMessage_QueueHandler_FromTerminal);
    mCANMessage_QueueHandler_FromTerminal = getSharedObject(shared_CAN_message_queue_terminal);

    //initialize switches
    SW.init();

    //initialize LD display
    LD.init();

    CAN_ST.init();

    printf("CONTROL HANDLER TASK INITIATED!\n");
    return true;
}

bool control_handler_task::run(void *p) {

    //read message_id_queue
    //depending of the message, a function that handles the states will be called
   // printf("control handler running\n");
   // uint8_t tempSensorValues[SIZE_OF_SENSOR_ARRAY] = { 0 };

    if(xQueueReceive(mCANMessage_QueueHandler, &mCAN_MSG_Rx, 0)) {
           // printf("Handler Received ID: 0x%03" PRIx32 "\n", mCAN_MSG_Rx.msg_id);

        //if Android sends a new command, stateCounters will be reset,

        switch (mCAN_MSG_Rx.msg_id) {
//            case (uint32_t) ANDROID_COMMANDS_MASTER:
//                    printf("ANDROID_COMMANDS_MASTER: 0x%03" PRIx32 "\n",mCAN_MSG_Rx.msg_id);
//                    if(mCAN_MSG_Rx.data.bytes[0] != (uint8_t) VALUE_NO_CHANGE) {
//                        //set mManualControlValue
//                        CAN_ST.setManualControlValue(mCAN_MSG_Rx.data.bytes[0]);
//                            if((uint8_t)MANUAL_CONTROL_ENABLED == CAN_ST.getManualControlValue())   {
//                                printf("Manual Control Enabled.\n");
//                                LOG_INFO("Manual Control Enabled");
//                            }
//                            else if(MANUAL_CONTROL_DISABLED == CAN_ST.getManualControlValue()) {
//                               printf("Manual Control Enabled.\n");
//                               LOG_DEBUG("Manual Control Enabled");
//                            }
//                    }
//                    else if (mCAN_MSG_Rx.data.bytes[1] != (uint8_t) VALUE_NO_CHANGE)   {
//                        //set Go Signal
//                        CAN_ST.setGoSignalValue(mCAN_MSG_Rx.data.bytes[1]);
//                            if(VALUE_TRUE == CAN_ST.getGoSignalValue()) {
//                                printf("GO signal set to TRUE\n");
//                                LOG_DEBUG("GO signal set to TRUE\n");
//                            }
//                            else if(VALUE_FALSE == CAN_ST.getGoSignalValue())   {
//                                printf("Go signal set to FALSE\n");
//                                LOG_DEBUG("Go signal set to FALSE\n");
//                            }
//                    }
//                    //else if read DATA
//                    //not yet implemented
//                    break;
//            case (uint32_t) RC_PARAMS:
//                    printf("RC_PARAMS: 0x%03" PRIx32 "\n",mCAN_MSG_Rx.msg_id);
//                    break;
//            //sends a request to sensor to read sensor data
//            case (uint32_t) MASTER_COMMANDS_SENSOR:
//                    printf("MASTER_COMMANDS_SENSOR: 0x%03" PRIx32 "\n",mCAN_MSG_Rx.msg_id);
//
//                    mCAN_MSG_Tx.data.bytes[0] = (uint8_t) READ_SENSOR_DATA;
//                    xQueueSend(mCANMessage_QueueHandler_Transmit, &mCAN_MSG_Tx, 0);
//                    break;

            //  0x702                Sensor          [   7   ][  6   ]   5   ][  4   ][ back ][right ][ mid  ][ left ]
            case (uint32_t) SENSOR_MASTER_REG:

                    //sen_msg_t *p = (sen_msg_t*) & mCAN_MSG_Rx.data.bytes[0];
                    printf("SENSOR_MASTER_REG: 0x%03" PRIx32 "\n",mCAN_MSG_Rx.msg_id);

                    //store our received data to our sensor_message
                    CAN_ST.sensor_data = (sen_msg_t*) & mCAN_MSG_Rx.data.bytes[0];

//                    for (int i = 0; i < (int)SIZE_OF_SENSOR_ARRAY; i++)     {
//                        tempSensorValues[i] = mCAN_MSG_Rx.data.bytes[i];
//                           // global_sensor_value[i] = mSensorValue[i];
//
//                        }
//                    CAN_ST.setSensorValues(tempSensorValues, (int) SIZE_OF_SENSOR_ARRAY);
                    break;
            // 0x704                Master          [   7   ][  6   ][  5   ][  4   ][  3   ][  SPD ][  L/R ][F/R/S ]
            case (uint32_t) MASTER_COMMANDS_MOTOR:
                    printf("MASTER_COMMANDS_MOTOR: 0x%03" PRIx32 "\n",mCAN_MSG_Rx.msg_id);
                    //copy the message to our tx frame
                    mCAN_MSG_Tx = mCAN_MSG_Rx;
                    xQueueSend(mCANMessage_QueueHandler_Transmit, &mCAN_MSG_Tx, 0);
                    break;

            default:
              //  printf("No applicable message ID's received.\n");
               // printf ("CAN_STORAGE MOTOR VALUE %d\n", CAN_ST.getMotorSpeed());
                break;

        }
    }

    vTaskDelay(1);
    return true;

}




