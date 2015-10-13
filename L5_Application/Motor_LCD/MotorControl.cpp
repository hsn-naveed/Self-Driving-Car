/*
 * MotorControl.cpp
 *
 * Created by Jashan and Hector
 *
 *
 */

#include "MotorControl.hpp"

#if 1 Private Functions
void MotorControl::setSteeringDirectionAndSpeed(){

}
#endif // Private Functions

#if 1 Public Functions
MotorControl::MotorControl(PWM &motorPwmToSet, PWM &servoPwmToSet, uint8_t priorityToUse){
currentMotorValue = speedSetting_t.STOP;
currentServoValue = steeringDirection_t.STRAIGHT;

motorHasBeenInitialized = false;
}

bool MotorControl::run(void *p){

}

void MotorControl::initCarMotor(){

}

#if 1   Motor Control Functions
void MotorControl::forward(int speed, int duration){

}

void MotorControl::back(int speed, int duration){

}
#endif // Motor Control Functions

#if 1   Servo Steering Functions
void MotorControl::steerLeft(float amountToSteer, float distance){

}

void MotorControl::steerRight(float amountToSteer, float distance){

}
#endif // Servo Steering Functions
#endif // Public Functions
