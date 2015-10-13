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

#define tx_android_src (1 << 3)
#define rx_android_dest (1 << 0)
#define dest_gps_id (1 << 2)
#define android_msg (0x8 << 10)


/// This is the stack size used for each of the period tasks
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);


// Homework 4 Filtered Light Sensor
#define hw4LightSensor 1
int filteredLightSensorValue = 0;
int sumOfLightSensorValues = 0;
int counterOfLightSensorValues = 0;


void averageOfLightSensor(){
    sumOfLightSensorValues += LS.getPercentValue();

    counterOfLightSensorValues++;

    if (counterOfLightSensorValues == 10){
        filteredLightSensorValue = sumOfLightSensorValues/counterOfLightSensorValues;

        LOG_INFO("\nCounter has reached 10!\n\n");
    }
    else{
        LOG_INFO("Counter not at 10 yet\n");
        filteredLightSensorValue = -1;
    }
}

void printFilteredLightSensorValue(){
    if (counterOfLightSensorValues == 10){
        printf("Filtered Light Sensor value = %i\n", filteredLightSensorValue);

        sumOfLightSensorValues = 0;
        counterOfLightSensorValues = 0;
    }
    else{
        LOG_INFO("Counter is not equal to 10 after 10 cycles of 10Hz func calls\n");
    }
}

void period_1Hz(void)
{
    LE.toggle(1);

#if hw4LightSensor
    printFilteredLightSensorValue();
#endif
}

void period_10Hz(void)
{
    can_msg_t msg_tx = {0};   // INITIALIZE

    uint32_t txID = android_msg | tx_android_src | dest_gps_id;
    /* Initialize and then transfer*/
    msg_tx.msg_id = txID;
    msg_tx.frame_fields.is_29bit = 0;
    msg_tx.frame_fields.data_len = 1;
    msg_tx.data.bytes[0] = 0;
    printf ("bout to send\n");
    CAN_tx(can1, &msg_tx, 100);
    printf ("sent\n");

    LE.toggle(2);

#if hw4LightSensor
    averageOfLightSensor();
#endif
}

void period_100Hz(void)
{
    LE.toggle(3);
}

void period_1000Hz(void)
{
    LE.toggle(4);
}
