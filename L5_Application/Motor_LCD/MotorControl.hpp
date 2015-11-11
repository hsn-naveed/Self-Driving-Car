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
#include "periodic_scheduler/periodic_callback.h"

#define MOTOR_SERVO_PWM_FREQ 100
#define MOTOR_INIT_NEEDED 1

struct{
        float SLOW_SPEED = 15.4205;
        float MEDIUM_SPEED = 15.45;
        float FAST_SPEED = 18;
        float NO_CHANGE = 1.0;
        float BACK_SPEED = 14;
        float BRAKE = 10;
        float MAX_SPEED = 20;
        float STOP = 15;
} speedSetting_t;

struct{
        float FULL_LEFT = 10;
        float FULL_RIGHT = 20;
        float STRAIGHT = 15;
} steeringDirection_t;

class MotorControl{
    private:
        // Objects used for setting PWM duty cycles
        PWM MotorPwm;
        PWM ServoPwm;

        // Variables used for keeping track of previous values, in case there's no change
        float CurrentMotorValue;
        float CurrentServoValue;

        // Sets the currentMotorValue, and currentServo value to
        //appropriate settings based on HEX values received
        float convertHexToFloatSpeed(uint8_t hexSpeedValue);
        float convertHexToFloatSteer(uint8_t hexSteerValue);

        // Used for changing between forward and reverse direction
        void changeMotorDirection(float speedToSet);

        #if MOTOR_INIT_NEEDED
        bool escHasBeenInitialized;
        #endif
    public:
        // Message structure from master_controller to motor
        mast_mot_msg_t *motorControlStruct = new mast_mot_msg_t {0};

        // Constructor to deal with scope issues
        // Pwm must be initialized outside (globally; e.g. in main.cpp) and passed in
        MotorControl(PWM &motorPwmToSet, PWM &servoPwmToSet);
        MotorControl();

        MotorControl operator=( MotorControl *obj){
            this->MotorPwm = obj->MotorPwm;
            this->ServoPwm = obj->ServoPwm;
            this->CurrentMotorValue = obj->CurrentMotorValue;
            this->CurrentServoValue = obj->CurrentServoValue;
            this->motorControlStruct = obj->motorControlStruct;
            this->escHasBeenInitialized = obj->escHasBeenInitialized;
        }

        #if MOTOR_INIT_NEEDED
        // Used for programming ESC at each startup
        void initCarMotor();
        #endif

        void getCANMessageData(can_fullcan_msg_t *fc1, mast_mot_msg_t *motorControlStruct);
        void convertFromHexAndApplyMotorAndServoSettings(mast_mot_msg_t *hexMotorControl);
        void setSteeringDirectionAndSpeed(float steeringDirectionToSet, float speedToSet);

        #if 0   // Motor Control Functions
        // Parameters are subject to change
        void forward(float speedToSet);
        void back(float speedToSet);
        #endif

        #if 0   // Servo Steering Functions
        // Parameters are subject to change
        void steerLeft(float amountToSteer);
        void steerRight(float amountToSteer);
        #endif
};
#endif
