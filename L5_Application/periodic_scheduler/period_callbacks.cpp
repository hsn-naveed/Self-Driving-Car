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
#include "L5_Application/can_message.h"
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

#include "Motor_LCD/LCD.hpp"


MotorControl motorObj;



can_fullcan_msg_t *canMsgForMotor = new can_fullcan_msg_t;
msg_hdr_t motorMessage = MASTER_TX_MOTOR_CMD_HDR;
//MASTER_TX_MOTOR_CMD_t *receivedMotorCommands = new MASTER_TX_MOTOR_CMD_t;



/// This is the stack size used for each of the period tasks
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

float incrementSpeedAmount = .5;

/// Called once before the RTOS is started, this is a good place to initialize things once
bool period_init(void)
{
    /// CAN bus initialization
    uint32_t std_list_arr[] = {(uint32_t)MASTER_TX_MOTOR_CMD_HDR.mid};

    size_t sizeOfArray = (sizeof(std_list_arr) / sizeof(*std_list_arr));

    iCAN_init_FULLCAN(std_list_arr, sizeOfArray);

    return true; // Must return true upon success
}

/// Register any telemetry variables
bool period_reg_tlm(void)
{
//    TLM_REG_VAR(tlm_component_get_by_name("disk"), MOTOR_SERVO_PWM_FREQ, tlm_float);
//
//    TLM_REG_VAR(tlm_component_get_by_name("disk"), MEDIUM_SPEED_OFFSET, tlm_float);
//    TLM_REG_VAR(tlm_component_get_by_name("disk"), SLOW_SPEED_OFFSET, tlm_float);
//    TLM_REG_VAR(tlm_component_get_by_name("disk"), BACK_SPEED_OFFSET, tlm_float);

    return true; // Must return true upon success
}

int count = 0;
void setSpeedAndIncrementCount(float speedToSet){
    motorObj.setSteeringDirectionAndSpeed(motorObj.STRAIGHT, speedToSet);
    printf("\nCount = %i, speed = %.5f\n\n",count++, speedToSet);
}

void period_1Hz(void)
{

}


void period_10Hz(void)
{
    if (CAN_is_bus_off(can1))
    {
        puts("====CAN BUS is off====\n");
        LE.on(led1);
    }
    else if (iCAN_rx(canMsgForMotor, &motorMessage)){
        MASTER_TX_MOTOR_CMD_decode(motorObj.receivedMotorCommands, &(canMsgForMotor->data.qword), &MASTER_TX_MOTOR_CMD_HDR);

//        printf("received steer = %i\n", receivedMotorCommands->MASTER_MOTOR_CMD_steer);
//                printf("received drive = %i\n", receivedMotorCommands->MASTER_MOTOR_CMD_drive);


        motorObj.convertFromIntegerAndApplyServoAndMotorSettings(motorObj.receivedMotorCommands);



//                motorObj.getCANMessageData(canMsgForMotor, motorObj.motorControlStruct);
//                motorObj.convertFromHexAndApplyMotorAndServoSettings(motorObj.motorControlStruct);

        LE.off(led1);
    }
    else
    {

        LE.on(led1);
    }


//    if (HasSpeedChanged() == 1){
//        /// Adjust motor speed offset accordingly
//        if ((motorObj.SLOW_SPEED + *SLOW_SPEED_OFFSET) <= motorObj.maxSlowSpeed){
//            *SLOW_SPEED_OFFSET += incrementSpeedAmount;
//            motorObj.SLOW_SPEED += *SLOW_SPEED_OFFSET;
//        }
//        printf("Inside hasSpeedChanged = 1\nMotorValue = %.5f\n\n", motorObj.SLOW_SPEED);
//
//        MEDIUM_SPEED_OFFSET += incrementSpeedAmount;
//        LE.off(led3);
//    }
//    else if (HasSpeedChanged() == 2){
//        // Anything below the max negative speed offset, it throws off the duty cycle
//        // and car motor is unpredictable
//        if (motorObj.SLOW_SPEED >= (motorObj.maxSlowSpeed - initialSlowSpeedOffset)){
//        //if (initialSlowSpeedOffset <= (*SLOW_SPEED_OFFSET - incrementSpeedAmount)){
//            *SLOW_SPEED_OFFSET -= incrementSpeedAmount;
//            motorObj.SLOW_SPEED += *SLOW_SPEED_OFFSET;
//            printf("Inside hasSpeedChanged = 2\nMotorValue = %.5f\n\n", motorObj.SLOW_SPEED);
//        }
//
//
//        MEDIUM_SPEED_OFFSET -= incrementSpeedAmount;
//        LE.off(led3);
//    }
//    else{
//        LE.on(led3);
//        printf("no speed change \n");
//    }
}

void period_100Hz(void)
{

}


void period_1000Hz(void)
{
//    if (HasSpeedChanged() == 1)
//    {
//        /// Adjust motor speed offset accordingly
//        SLOW_SPEED_OFFSET += incrementSpeedAmount;
//        MEDIUM_SPEED_OFFSET += incrementSpeedAmount;
//    }
//    else if (HasSpeedChanged() == 2)
//    {
//        // Anything below the max negative speed offset, it throws off the duty cycle
//        // and car motor is unpredictable
//        if (SLOW_SPEED_OFFSET
//                < (maxNegativeSlowSpeedOffset - incrementSpeedAmount))
//        {
//            SLOW_SPEED_OFFSET -= incrementSpeedAmount;
//        }
//        MEDIUM_SPEED_OFFSET -= incrementSpeedAmount;
//    }
}
