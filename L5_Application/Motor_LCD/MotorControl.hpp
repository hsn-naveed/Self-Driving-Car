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
#define MOTOR_SERVO_PWM_FREQ 100

/// For dynamically calculating
#define ONE_SECOND_MS 1000
#define NEUTRAL_PWM_PERIOD_MS 1.5
#define FORWARD_PWM_PERIOD_MS 2
#define REVERSE_PWM_PERIOD_MS 1

/*
 * @about For precise tweaking of speed
 *
 * @warning Make sure to use negative for medium and slow
 * and positive for back speed, as it is probably more likely
 * that the speeds are too fast
 */
#define MEDIUM_SPEED_OFFSET 0
#define SLOW_SPEED_OFFSET 0
#define BACK_SPEED_OFFSET 0

/// For programming ESC
#define ESC_INIT_NEEDED 1

struct{
        float pwmFreqInMs = ((float)ONE_SECOND_MS)/(float)MOTOR_SERVO_PWM_FREQ;

        float FAST_SPEED = ((float)FORWARD_PWM_PERIOD_MS)/pwmFreqInMs;
        float BRAKE = ((float)REVERSE_PWM_PERIOD_MS)/pwmFreqInMs;
        float MEDIUM_SPEED = (FAST_SPEED+NEUTRAL)/2 + (float)MEDIUM_SPEED_OFFSET;
        float SLOW_SPEED = (MEDIUM_SPEED+NEUTRAL)/2 + (float)SLOW_SPEED_OFFSET;
        float BACK_SPEED = ((float)BRAKE+NEUTRAL)/2 + (float)BACK_SPEED_OFFSET;
        float NEUTRAL = ((float)NEUTRAL_PWM_PERIOD_MS)/pwmFreqInMs;
} speedSetting_t;

struct{
        float FULL_LEFT = 10;
        float FULL_RIGHT = 20;
        float STRAIGHT = 15;
} steeringDirection_t;

class MotorControl{
    private:
        /// Objects used for setting PWM duty cycles
        PWM MotorPwm;
        PWM ServoPwm;

        /// Variables used for keeping track of previous values, in case there's no change
        float CurrentMotorValue;
        float CurrentServoValue;

        /// Sets the currentMotorValue, and currentServo value to
        // appropriate settings based on HEX values received
        float convertHexToFloatSpeed(uint8_t hexSpeedValue);
        float convertHexToFloatSteer(uint8_t hexSteerValue);

        /// Used for changing between forward and reverse direction
        void changeMotorDirection(float speedToSet);

#if ESC_INIT_NEEDED
        void triggerForwardOrReverseThrottle(float maxOrMin,
                                                double incrementAndDecrementSize,
                                                int pwmDelay);

        bool escHasBeenInitialized;
#endif
    public:
        /// Message structure from master_controller to motor
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

#if ESC_INIT_NEEDED
        /// Used for programming ESC, if needed
        void initCarMotor();
#endif


        /// Functions for using CAN message, and setting appropriate duty cycles and sending those pwm signals
        void getCANMessageData(can_fullcan_msg_t *fc1, mast_mot_msg_t *motorControlStruct);
        void convertFromHexAndApplyMotorAndServoSettings(mast_mot_msg_t *hexMotorControl);
        void setSteeringDirectionAndSpeed(float steeringDirectionToSet, float speedToSet);
};
#endif
