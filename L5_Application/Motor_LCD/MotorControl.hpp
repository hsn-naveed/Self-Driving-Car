/*
 * MotorControl.hpp
 *
 * Created by Jashan and Hector
 *
 *
 */

#ifndef MOTORCONTROL_HPP_
#define MOTORCONTROL_HPP_

#include "lpc_pwm.hpp"
#include "scheduler_task.hpp"
#include "FreeRTOS.h"
#include "io.hpp"
#include "file_logger.h"
#include <stdio.h>
#include "can.h"
#include "utilities.h"
#include "CAN_structs.h"
#define MOTOR_PWM_FREQ 100
#define SERVO_PWM_FREQ 50
#define MOTOR_INIT_NEEDED 1


struct{
        float SLOW_SPEED = 16;
        float MEDIUM_SPEED = 16.5;
        float FAST_SPEED = 18;
        float NO_CHANGE = 1.0;
        float BACK_SPEED = 14;
        float MAX_SPEED = 20;
        float STOP = 15;
} speedSetting_t;

struct{
        float FULL_LEFT = 96;
        float FULL_RIGHT = 48;
        float STRAIGHT = 72;
} steeringDirection_t;

class MotorControl{
    private:
        PWM motorPwm;
        PWM servoPwm;

        float currentMotorValue;
        float currentServoValue;

        void setSteeringDirectionAndSpeed(float steeringDirectionToSet, float speedToSet);

        #if MOTOR_INIT_NEEDED
        bool escHasBeenInitialized;
        #endif
    public:
        mast_mot_msg_t *motor_control_struct;

        MotorControl(PWM &motorPwmToSet, PWM &servoPwmToSet);
        MotorControl();

        MotorControl operator=( MotorControl *obj){
            this->motorPwm = obj->motorPwm;
            this->servoPwm = obj->servoPwm;
            this->currentMotorValue = obj->currentMotorValue;
            this->currentServoValue = obj->currentServoValue;
            this->motor_control_struct = obj->motor_control_struct;
            this->escHasBeenInitialized = obj->escHasBeenInitialized;
        }

        #if MOTOR_INIT_NEEDED
        // In case motor needs to be pulsed to get going prior to starting
        void initCarMotor();
        #endif

        #if 1   // Motor Control Functions
        // Parameters are subject to change
        void forward(float speedToSet);
        void back(float speedToSet);

        void getCANMessageData(can_msg_t *fc1, mast_mot_msg_t *motorControlStruct);
        void convertHexToDutyCycle(mast_mot_msg_t *hexMotorControl);
        #endif

        #if 1   // Servo Steering Functions
        // Parameters are subject to change
        void steerLeft(float amountToSteer);
        void steerRight(float amountToSteer);
        #endif
};
#endif
