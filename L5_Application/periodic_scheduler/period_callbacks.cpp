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
#include "L5_Application/can_message.h"
#include "Motor_LCD/LCD.hpp"


/// Object used for motor and servo control
MotorControl motorObj;
LCD lcdObj;

/// These variables are used for CAN communication
can_fullcan_msg_t *canMsgForMotor = new can_fullcan_msg_t;
msg_hdr_t motorMessage = MASTER_TX_MOTOR_CMD_HDR;
msg_hdr_t gpsHeadingMessage = GPS_TX_INFO_HEADING_HDR;


/// This is the stack size used for each of the period tasks
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

float incrementSpeedAmount = .2;

/// Called once before the RTOS is started, this is a good place to initialize things once
bool period_init(void)
{
    /// CAN bus initialization
    uint32_t std_list_arr[] = {MASTER_TX_MOTOR_CMD_HDR.mid,GPS_TX_INFO_HEADING_HDR.mid};

    size_t sizeOfArray = (sizeof(std_list_arr) / sizeof(*std_list_arr));

    iCAN_init_FULLCAN(std_list_arr, sizeOfArray);

    /// Clear LCD in case there's stuff on LCD left over
    lcdObj.clearLCD();
    lcdObj.initLCD();
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
void period_1Hz(void)
{

//    if (count == 0){
//        motorObj.setSteeringDirectionAndSpeed(motorObj.STRAIGHT, motorObj.MEDIUM_SPEED);
//        count++;
//    }
//
//    if (count >= 5){
//        motorObj.setSteeringDirectionAndSpeed(motorObj.STRAIGHT, motorObj.BRAKE);
//        count = 0;
//    }
}


void period_10Hz(void)
{
//    motorObj.setSteeringDirectionAndSpeed(motorObj.STRAIGHT, motorObj.MEDIUM_SPEED);


    if (CAN_is_bus_off(can1)){
        puts("====CAN BUS is off====\n");
          lcdObj.toggleLCDBrightness();
        LE.on(led1);
    }
    else if (iCAN_rx(canMsgForMotor, &motorMessage)){
//        portDISABLE_INTERRUPTS();
        MASTER_TX_MOTOR_CMD_decode(motorObj.receivedMotorCommands, &(canMsgForMotor->data.qword), &MASTER_TX_MOTOR_CMD_HDR);
//        portENABLE_INTERRUPTS();

        motorObj.convertFromIntegerAndApplyServoAndMotorSettings(motorObj.receivedMotorCommands);
          lcdObj.getMessageDataFromMotor(motorObj.receivedMotorCommands);
        LE.off(led1);
    }
    else{
        LE.on(led1);
    }


//    /// Increase PWM Duty cycle (speed up) because car is slowing down
//    if (HasSpeedChanged() == SPEED_BELOW_THRESHOLD_SLOW){
//        /// Adjust motor speed offset accordingly
//        motorObj.SLOW_SPEED += incrementSpeedAmount;
//        LE.on(led3);
//    }
//    else if (HasSpeedChanged() == SPEED_BELOW_THRESHOLD_MEDIUM){
//        motorObj.MEDIUM_SPEED += incrementSpeedAmount;
//        LE.on(led3);
//    }
//
//    /// Decrease PWM duty cycle because car is moving too fast
//    else if (HasSpeedChanged() == SPEED_ABOVE_THRESHOLD_SLOW){
//        if (motorObj.SLOW_SPEED >= (motorObj.maxSlowSpeed)){
//            incrementSpeedAmount = motorObj.SLOW_SPEED - motorObj.maxSlowSpeed;
//            motorObj.SLOW_SPEED += incrementSpeedAmount;
//
//            /// Reset back to original increment amount in case it is greater or smaller
//            incrementSpeedAmount = .2;
//        }
//    }
//    else if (HasSpeedChanged() == SPEED_ABOVE_THRESHOLD_MEDIUM){
//        if (motorObj.MEDIUM_SPEED >= motorObj.SLOW_SPEED){
//            motorObj.MEDIUM_SPEED -= incrementSpeedAmount;
//            LE.on(led3);
//        }
//    }
//    else{
//        LE.off(led3);
//        printf("No speed change \n");
//    }
}

void period_100Hz(void)
{
}


void period_1000Hz(void)
{
}
