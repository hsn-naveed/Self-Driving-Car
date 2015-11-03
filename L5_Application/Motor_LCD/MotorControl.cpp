/*
 * MotorControl.cpp
 *
 * Created by Jashan and Hector
 *
 *
 */

#include "MotorControl.hpp"
#include "savedMotorData.cpp"
#include "CAN_structs.h"
// Just for regioning code
#define Private_Functions 1
#define Public_Functions 1
#define Motor_Control_Functions 1
#define Servo_Steering_Functions 1

#define Motor_LCD_controller_ID 0x3F2 //1010

#if Private_Functions
void MotorControl::setSteeringDirectionAndSpeed(float steeringDirectionToSet, float speedToSet){
    currentServoValue = steeringDirectionToSet;
    currentMotorValue = speedToSet;

    motorPwm.set(currentMotorValue);
    servoPwm.set(currentServoValue);
}
#endif // Private Functions

#if Public_Functions
MotorControl::MotorControl():
    motorPwm(PWM(PWM::pwm1, frequency)),
    servoPwm(PWM(PWM::pwm2, frequency)){
    currentMotorValue = speedSetting_t.STOP;
    currentServoValue = steeringDirection_t.STRAIGHT;

#if MOTOR_INIT_NEEDED
    motorHasBeenInitialized = false;
#endif
}

#if MOTOR_INIT_NEEDED
void MotorControl::initCarMotor(){
    if (motorHasBeenInitialized == false){
            float startingSpeed = speedSetting_t.SLOW_SPEED - 1.3;

            for (int i = 0; i < 3; i++){
                MotorControl::forward(startingSpeed);
                MotorControl::forward(startingSpeed);
                delay_ms(100);
            }
    }
}
#endif

#if Motor_Control_Functions
void MotorControl::forward(float speedToSet){
    printf("-----Moving Forward--------\n");

    if (speedToSet == speedSetting_t.NO_CHANGE){
        setSteeringDirectionAndSpeed(steeringDirection_t.STRAIGHT, currentMotorValue);
    }
    else{
        setSteeringDirectionAndSpeed(steeringDirection_t.STRAIGHT, speedToSet);
    }
}

void MotorControl::back(float speedToSet){
    printf("----Moving Backward-------\n");

    if (speedToSet != speedSetting_t.BACK_SPEED){
     // Should never get here
        while(1){
            printf("BACK SPEED NOT SET CORRECTLY");
            LE.on(1);
            setSteeringDirectionAndSpeed(steeringDirection_t.STRAIGHT, speedSetting_t.STOP);
            LOG_INFO("Back speed not set correctly!");
        }
    }
    else{
        setSteeringDirectionAndSpeed(steeringDirection_t.STRAIGHT, speedToSet);
    }
}
#endif // Motor Control Functions

#if Servo_Steering_Functions
void MotorControl::steerLeft(float amountToSteer){
    printf("----Turning Left----");

    setSteeringDirectionAndSpeed(steeringDirection_t.FULL_LEFT, speedSetting_t.MEDIUM_SPEED);
}

void MotorControl::steerRight(float amountToSteer){
    printf("----Turning Right----");

    setSteeringDirectionAndSpeed(steeringDirection_t.FULL_RIGHT, speedSetting_t.MEDIUM_SPEED);
}
#endif // Servo Steering Functions
#endif // Public Functions


void getData(can_fullcan_msg_t *fc1)
{

    CAN_motor_data_struct *p;
    mast_mot_msg_t *dataStruct;


        p->motor_direction = dataStruct->FRS;
        p->servo_direction = dataStruct->LR;
        p->motor_speed = dataStruct->SPD;


}

