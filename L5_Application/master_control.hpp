/*
 * master_control.hpp
 *
 *  Created on: Oct 24, 2015
 *      Author: Marvin
 *
 *      This File contains most of the tasks needed for
 *      self-driving vehicle
 */

#ifndef L5_APPLICATION_MASTER_CONTROL_HPP_
#define L5_APPLICATION_MASTER_CONTROL_HPP_

#include "scheduler_task.hpp"
#include "shared_handles.h"
#include "uart3.hpp"
#include "examples/rn_xv_task.hpp"
#include "FreeRTOS.h"
#include "semphr.h"
#include "can.h"

//modes
#define STANDBY_MODE 0x00
#define MANUAL_MODE 0x01
#define AUTO_MODE 0x02
#define FREERUN_MODE 0x03

#define BROADCAST_ID 0x00
#define ANDROID_ID 0x01
#define MASTER_ID 0x02
#define SENSOR_ID 0x03
#define GPS_ID 0x04
#define MOTOR_IO_ID 0x05

//message id's
//Using <source>_<destination> labeling
#define CRITICAL_ERROR_ANDROID 0x008
#define CRTICAL_ERROR_MASTER 0x010
#define CRITICAL_ERROR_SENSOR 0x018
#define CRITICAL_ERROR_GPS 0x020
#define CRITICAL_ERROR_MOTOR_IO 0x028

#define ANDROID_COMMANDS_MASTER 0x04A
#define RC_PARAMS 0x08A
#define SET_DEST 0x0CA
#define COORDINATES_ANDROID_MASTER 0x10A
#define SENSOR_MASTER_REG 0x14A
#define MASTER_ANDROID_REG 0x191
#define COORDINATES_MASTER_ANDROID 0x1D1
#define MASTER_COMMANDS_MOTOR 0x215
#define MASTER_COMMANDS_SENSOR 0x253
#define MASTER_COMMANDS_READ_IO 0x295
#define IO_MOTOR_MASTER_REG 0x2D5
#define MASTER_COMMANDS_GPS 0x314
#define GPS_MASTER_REG 0x362
#define MASTER_COMMANDS_WRITE_IO 0x395

#define HEARTBEAT_ANDROID 0x3CA
#define HEARTBEAT_SENSOR 0x41A
#define HEARTBEAT_GPS 0x462
#define HEARTBEAT_IO_MOTOR 0x4AA

#define HEARBEAT_VALUES 0x4D0

#define VALUE_TRUE 0xFF
#define VALUE_FALSE 0x11
#define VALUE_NO_CHANGE 0x00

//Sensor
#define LEFT_SENSOR 0
#define MIDDLE_SENSOR 1
#define RIGHT_SENSOR 2
#define BACK_SENSOR 3

#define SIZE_OF_SENSOR_ARRAY 4
#define READ_SENSOR_DATA 0x01

//int global_sensor_value[(int)SIZE_OF_SENSOR_ARRAY];

class control_handler_task : public scheduler_task
{
    public:
        control_handler_task(uint8_t = 5);
        bool init();
        bool run(void *p);


    private:

        //Queue Handlers
        QueueHandle_t mCANMessage_QueueHandler; //default message queue for the system

        QueueHandle_t mCANMessage_QueueHandler_FromTerminal; //CAN message from terminal for debugging
        QueueHandle_t mCANMessage_QueueHandler_Receive; //CAN message from receiving task/ CAN handler task

        QueueHandle_t mCANMessage_QueueHandler_Transmit;//CAN message to be transmitted

        QueueHandle_t mSensorData_QueueHandler;

        //Actual receive and send data
        can_msg_t mCAN_MSG_Rx; //current Data Received
        can_msg_t mCAN_MSG_Tx; //current Data to Send

        int mMode;

        //state counter
        volatile int mStateCount_Motor;
        volatile int mStateCount;
        volatile int mStateCount_Sensor;


        //Android
        const uint8_t MANUAL_CONTROL_ENABLED = 0xFF;
        const uint8_t MANUAL_CONTROL_DISABLED = 0x11;

        //byte[0] of ANDROID_COMMANDS -> MAS
        //ENABLE: 0xFF
        //DISABLE: 0x11
        uint8_t mManualControlValue;
        uint8_t mGoSignal;

        //GPS
        uint32_t mCheckPointX[8]; //contains X coordinates of the checkpoints. 8 max check points supported
        uint32_t mCheckPointY[8]; //contains Y coordinates of the checkpoints

        //if there's only one checkpoint, mCheckPoint[0] will be equal to mFinalDestination
        uint32_t mFinalDestinationX; //contains X coordinates of the final destination
        uint32_t mFinalDestinationY; //contains Y coordinates of the final destination
        uint16_t mAngleValue; //angle value with respect to the current checkpoint

        //Sensor
        uint8_t mSensorValue[(int)SIZE_OF_SENSOR_ARRAY]; //current sensor values; 4 total sensors

        //Motor
        int mMotorSpeed;
        int mMotorDirection;
        int mMotorTurn;
};



#endif /* L5_APPLICATION_MASTER_CONTROL_HPP_ */
