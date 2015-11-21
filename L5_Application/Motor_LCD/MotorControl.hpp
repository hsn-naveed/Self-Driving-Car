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


/// Used for PWM
static float MOTOR_SERVO_PWM_FREQ = 100;

/// For dynamically calculating
static float ONE_SECOND_MS = 1000;
static float STRAIGHT_PWM_PERIOD_MS = 1.5, NEUTRAL_PWM_PERIOD_MS = 1.5;
static float FULL_RIGHT_PWM_PERIOD_MS = 2, FORWARD_PWM_PERIOD_MS = 2;
static float FULL_LEFT_PWM_PERIOD_MS = 1, REVERSE_PWM_PERIOD_MS = 1;

/*
 * @about For precise tweaking of speed
 *
 * @warning Make sure to use negative for medium and slow
 * and positive for back speed, as it is probably more likely
 * that the speeds are too fast
 */
static float MEDIUM_SPEED_OFFSET = 0;
static float SLOW_SPEED_OFFSET = -.81;//-2.38;
static float BACK_SPEED_OFFSET = 0;

class MotorControl{
    private:
        /// Initialize globals
        void initAllGlobalsForMotorControl();

        /// Objects used for setting PWM duty cycles
        PWM MotorPwm;
        PWM ServoPwm;

        /// Variables used for keeping track of current motor and servo values, in case there's no change
        float CurrentMotorValue;
        float CurrentServoValue;


        /// Sets the currentMotorValue, and currentServo value to
        // appropriate settings based on HEX values received
        float convertHexToFloatSpeed(uint8_t hexSpeedValue);
        float convertHexToFloatSteer(uint8_t hexSteerValue);

        /// Used for changing between forward and reverse direction
        void changeMotorDirection(float speedToSet);


        /// Used for ESC programming
        void triggerForwardOrReverseThrottle(float maxOrMin,
                double incrementAndDecrementSize,
                int pwmDelay);
        bool escHasBeenInitialized;

    public:
        /// Message structure from master_controller to motor
        mast_mot_msg_t *motorControlStruct = new mast_mot_msg_t {0};

        // Constructor to deal with scope issues
        // Pwm must be initialized outside (globally; e.g. in main.cpp) and passed in
        MotorControl(PWM &motorPwmToSet, PWM &servoPwmToSet);
        MotorControl();

        MotorControl operator=(MotorControl *obj){
            this->MotorPwm = obj->MotorPwm;
            this->ServoPwm = obj->ServoPwm;
            this->CurrentMotorValue = obj->CurrentMotorValue;
            this->CurrentServoValue = obj->CurrentServoValue;
            this->motorControlStruct = obj->motorControlStruct;
            this->escHasBeenInitialized = obj->escHasBeenInitialized;
        }

        /// Used for programming ESC, if needed
        void initESC();


        /// Functions for using CAN message, and setting appropriate duty cycles and sending those pwm signals
        void getCANMessageData(can_fullcan_msg_t *fc1, mast_mot_msg_t *motorControlStruct);
        void convertFromHexAndApplyMotorAndServoSettings(mast_mot_msg_t *hexMotorControl);
        void setSteeringDirectionAndSpeed(float steeringDirectionToSet, float speedToSet);


        /// Speed setting
        float pwmFreqInMs = (ONE_SECOND_MS)/MOTOR_SERVO_PWM_FREQ;

        float FAST_SPEED = ((FORWARD_PWM_PERIOD_MS)/pwmFreqInMs) * 100;
        float BRAKE = ((REVERSE_PWM_PERIOD_MS)/pwmFreqInMs) * 100;
        float NEUTRAL = ((NEUTRAL_PWM_PERIOD_MS)/pwmFreqInMs) * 100;
        float MEDIUM_SPEED = (FAST_SPEED+NEUTRAL)/2 + MEDIUM_SPEED_OFFSET;
        float SLOW_SPEED = (MEDIUM_SPEED+NEUTRAL)/2 + SLOW_SPEED_OFFSET;
        float BACK_SPEED = (BRAKE+NEUTRAL)/2 + BACK_SPEED_OFFSET;

        /// Steering direction
        float FULL_LEFT = (FULL_LEFT_PWM_PERIOD_MS/pwmFreqInMs) * 100;
        float FULL_RIGHT = (FULL_RIGHT_PWM_PERIOD_MS/pwmFreqInMs) * 100;
        float STRAIGHT = (STRAIGHT_PWM_PERIOD_MS/pwmFreqInMs) * 100;
};
#endif
