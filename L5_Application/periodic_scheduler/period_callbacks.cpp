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
#include "CAN_structs.h"
#include "iCAN.hpp"
#include "can.h"
#include "Motor_LCD/MotorControl.hpp"

#define PWM_FREQ 100

#define LED_FLASH 1

extern MotorControl motorObj;
can_fullcan_msg_t *canMsgForMotor = { 0 };
uint16_t motorMsgId = 0x704;

/// This is the stack size used for each of the period tasks
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

#if LED_FLASH
enum{
    led1 = 1,
    led2 = 2,
    led3 = 3,
    led4 = 4
}led_num_t;

void flashLed(int ledNum, int numTimeToFlash = 4, int delaySpeedBetweenFlashes = 100){
    if (numTimeToFlash > 4 || numTimeToFlash < 0){
        numTimeToFlash = 4;
        for (int i = 0; i < numTimeToFlash; i++){
            LE.on(ledNum);
            delay_ms(delaySpeedBetweenFlashes);
            LE.off(ledNum);
            delay_ms(delaySpeedBetweenFlashes);
        }
    }
}
#endif

void period_1Hz(void)
{
    if (CAN_is_bus_off(can1)){
        puts("====CAN BUS is off====\n");
        LE.on(led1);
    }
    else if (iCAN_rx(canMsgForMotor, motorMsgId)){
        motorObj.getCANMessageData(canMsgForMotor, motorObj.motorControlStruct);
        motorObj.convertHexToDutyCycle(motorObj.motorControlStruct);
        LE.off(led1);
    }
    else{
        puts("Nothing received from CAN BUS\n");
        flashLed(led1);
    }
}

void period_10Hz(void)
{
    LE.toggle(2);
}

void period_100Hz(void)
{
    LE.toggle(3);
}

void period_1000Hz(void)
{
    LE.toggle(4);
}
