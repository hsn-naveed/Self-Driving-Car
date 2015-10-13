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
//another comment


//added comment

class MotorControl : public scheduler_task{
    public:
        PWM motorPwm;
        PWM servoPwm;
        float servoValue;
        float motorValue;

        MotorControl(PWM &motorPwmToSet, PWM &servoPwmToSet, float motorValueToSet, float servoValueToSet, uint8_t priorityToUse);
        bool run(void *p);

        // In case motor needs to be pulsed to get going prior to starting
        void initCarMotor();

        #if 1   // Motor Control Functions

        // Parameters are subject to change
        void forward(int speed, int duration);
        void back(int speed, int duration);
        #endif
};
