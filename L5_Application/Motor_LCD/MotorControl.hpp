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

struct{
        float SLOW_SPEED = 76.7;
        float FAST_SPEED = 79.3;
        float BACK_SPEED = 70;
        float MAX_SPEED = 96;
        float STOP = 71.3;
} speedSetting_t;

struct{
        float FULL_LEFT = 96;
        float FULL_RIGHT = 48;
        float STRAIGHT = 72;
} steeringDirection_t;

class MotorControl : public scheduler_task{
    private:
        float currentMotorValue;
        float currentServoValue;

        void setSteeringDirectionAndSpeed();

        bool motorHasBeenInitialized = false;
    public:
        PWM motorPwm;
        PWM servoPwm;

        MotorControl(PWM &motorPwmToSet, PWM &servoPwmToSet, uint8_t priorityToUse);
        bool run(void *p);

        // In case motor needs to be pulsed to get going prior to starting
        void initCarMotor();

        #if 1   // Motor Control Functions
        // Parameters are subject to change
        void forward(int speed, int duration);
        void back(int speed, int duration);
        #endif

        #if 1   // Servo Steering functions
        // Parameters are subject to change
        void steerLeft(float amountToSteer, float distance);
        void steerRight(float amountToSteer, float distance);
        #endif
};
