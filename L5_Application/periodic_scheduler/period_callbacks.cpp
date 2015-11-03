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

can_fullcan_msg_t *message_struct;
mast_mot_msg_t *motor_control_struct;


/// This is the stack size used for each of the period tasks
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);


MotorControl motorObj;
bool motorInit = false;

PWM pwmMotorObj = PWM(PWM::pwm1, 480);
float counter = 71.3;
bool motorHasBeenSet = false;

void period_1Hz(void)
{
//    if (counter < 100){
//            //if (!motorHasBeenSet){
//                pwmMotorObj.set(counter);
//                motorHasBeenSet = true;
//            //}
//
//            //        counter += 1;
//            printf("Sent forward speed to motor: %.2f\n", counter);
//        }
//        else{
//            puts("No servo values worked");
//        }
//
//    #if 0
//        if (!motorInit){
//            motorObj.initCarMotor();
//            motorInit = true;
//            puts("motor has been initialized");
//        }
//        motorObj.forward(speedSetting_t.SLOW_SPEED);
//    #endif
    LE.toggle(1);
}

void period_10Hz(void)
{


    LE.toggle(2);
    if(iCAN_rx(message_struct, 0x704)){
        motorObj.getData(message_struct, motor_control_struct);


    }
}

void period_100Hz(void)
{
    pwmMotorObj.set(counter);
    LE.toggle(3);
}

void period_1000Hz(void)
{
    LE.toggle(4);
}
