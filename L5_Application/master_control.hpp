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

#include <L4_IO/can_definitions.hpp>
#include "L4_IO/can_storage.hpp"

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




        //Motor
        //int mMotorSpeed;
        //int mMotorDirection;
        //int mMotorTurn;
};



#endif /* L5_APPLICATION_MASTER_CONTROL_HPP_ */
