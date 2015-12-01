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
#include "tlm/c_tlm_comp.h"
#include "tlm/c_tlm_var.h"
#include "CAN_structs.h"
#include "iCAN.hpp"
#include "Motor_LCD/MotorControl.hpp"
#include "Motor_LCD/MotorEncoder.hpp"

/// Object that will be used for calling all functions
   // related to motor and servo
MotorControl motorObj;

/// These variables are used for CAN bus communication
uint16_t motorMsgId = 0x704;
can_fullcan_msg_t *canMsgForMotor = new can_fullcan_msg_t {0};


/// This is the stack size used for each of the period tasks
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

/// Called once before the RTOS is started, this is a good place to initialize things once
bool period_init(void)
{
    /// CAN bus initialization
    uint16_t std_list_arr[] = {0x704};
    size_t sizeOfArray = (sizeof(std_list_arr) / sizeof(*std_list_arr));

    iCAN_init_FULLCAN(std_list_arr, sizeOfArray);

    return true; // Must return true upon success
}

/// Register any telemetry variables
bool period_reg_tlm(void){
//    TLM_REG_VAR(tlm_component_get_by_name("disk"), MOTOR_SERVO_PWM_FREQ, tlm_float);
//
//    TLM_REG_VAR(tlm_component_get_by_name("disk"), MEDIUM_SPEED_OFFSET, tlm_float);
//    TLM_REG_VAR(tlm_component_get_by_name("disk"), SLOW_SPEED_OFFSET, tlm_float);
//    TLM_REG_VAR(tlm_component_get_by_name("disk"), BACK_SPEED_OFFSET, tlm_float);

    return true; // Must return true upon success
}

void period_1Hz(void)
{

}

void period_10Hz(void)
{
//    motorObj.setSteeringDirectionAndSpeed(motorObj.STRAIGHT, motorObj.SLOW_SPEED);
}

void period_100Hz(void)
{
    /// FIX set this
        if (CAN_is_bus_off(can1)){
            puts("====CAN BUS is off====\n");
            LE.on(led1);
        }
        else if (iCAN_rx(canMsgForMotor, motorMsgId)){
            motorObj.getCANMessageData(canMsgForMotor, motorObj.motorControlStruct);
            motorObj.convertFromHexAndApplyMotorAndServoSettings(motorObj.motorControlStruct);
            LE.off(led1);
        }
        else{
            LE.on(led1);
        }
}

float incrementSpeedAmount = .5;
void period_1000Hz(void)
{
    if (HasSpeedChanged() == 1){
        /// Adjust motor speed offset accordingly
        SLOW_SPEED_OFFSET += incrementSpeedAmount;
        motorObj.SLOW_SPEED += SLOW_SPEED_OFFSET;
        MEDIUM_SPEED_OFFSET += incrementSpeedAmount;
    }
    else if (HasSpeedChanged() == 2){
        // Anything below the max negative speed offset, it throws off the duty cycle
        // and car motor is unpredictable
        if (SLOW_SPEED_OFFSET < ((-.81)- incrementSpeedAmount)){
            SLOW_SPEED_OFFSET -= incrementSpeedAmount;
        }
        MEDIUM_SPEED_OFFSET -= incrementSpeedAmount;
    }

}
