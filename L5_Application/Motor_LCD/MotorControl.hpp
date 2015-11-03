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
#define INIT_HZ 500
#define MOTOR_INIT_NEEDED 0


struct{
        float SLOW_SPEED = 57.0;
        float MEDIUM_SPEED = 70.0;
        float FAST_SPEED = 90.0;
        float NO_CHANGE = 1.0;
        float BACK_SPEED = 57.0;
        float MAX_SPEED = 96;
        float STOP = 0;
} speedSetting_t;

struct{
        float FULL_LEFT = 96;
        float FULL_RIGHT = 48;
        float STRAIGHT = 72;
} steeringDirection_t;

class MotorControl{
    private:
        int frequency = INIT_HZ;
        float currentMotorValue;
        float currentServoValue;

        void setSteeringDirectionAndSpeed(float steeringDirectionToSet, float speedToSet);

        #if MOTOR_INIT_NEEDED
        bool motorHasBeenInitialized;
        #endif
    public:
        PWM motorPwm;
        PWM servoPwm;

        MotorControl();

        #if MOTOR_INIT_NEEDED
        // In case motor needs to be pulsed to get going prior to starting
        void initCarMotor();
        #endif

        #if 1   // Motor Control Functions
        // Parameters are subject to change
        void forward(float speedToSet);
        void back(float speedToSet);

        void getData(can_fullcan_msg_t *fc1);
        #endif

        #if 1   // Servo Steering Functions
        // Parameters are subject to change
        void steerLeft(float amountToSteer);
        void steerRight(float amountToSteer);
        #endif
};
#endif
