/*
 * MotorControl.cpp
 *
 * Created by Jashan and Hector
 *
 *
 */

#include "MotorControl.hpp"

#if 1 Private Functions
void MotorControl::setSteeringDirectionAndSpeed(float steeringDirectionToSet, float speedToSet){
    currentServoValue = steeringDirectionToSet;
    currentMotorValue = speedToSet;

    motorPwm.set(currentMotorValue);
    servoPwm.set(currentServoValue);
}
#endif // Private Functions

#if 1 Public Functions
MotorControl::MotorControl(){
    motorPwm = PWM(PWM::pwm1, frequency);
    servoPwm = PWM(PWM::pwm2, frequency);

    currentMotorValue = speedSetting_t.STOP;
    currentServoValue = steeringDirection_t.STRAIGHT;

#if MOTOR_INIT_NEEDED
    motorHasBeenInitialized = false;
#endif
}

#if MOTOR_INIT_NEEDED
void MotorControl::initCarMotor(){

}
#endif

#if 1   Motor Control Functions
void MotorControl::forward(int speed, int duration){
    printf("-----Moving Forward--------");

    setSteeringDirectionAndSpeed(steeringDirection_t.STRAIGHT, speedSetting_t.FAST_SPEED);
}

void MotorControl::back(int speed, int duration){
    printf("----Moving Backward-------");

    setSteeringDirectionAndSpeed(steeringDirection_t.STRAIGHT, speedSetting_t.BACK_SPEED);
}
#endif // Motor Control Functions

#if 1   Servo Steering Functions
void MotorControl::steerLeft(float amountToSteer, float distance){

}

void MotorControl::steerRight(float amountToSteer, float distance){

}
#endif // Servo Steering Functions
#endif // Public Functions
