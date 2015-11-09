/*
 * MotorControl.cpp
 *
 * Created by Jashan and Hector
 *
 *
 */

#include "MotorControl.hpp"
// Just for regioning code
#define Motor_Control_Functions 0
#define Servo_Steering_Functions 0

#if 1 // Public Functions
MotorControl::MotorControl(PWM &motorPwmToSet, PWM &servoPwmToSet){
    MotorPwm = motorPwmToSet;
    ServoPwm = servoPwmToSet;

    CurrentMotorValue = speedSetting_t.STOP;
    CurrentServoValue = steeringDirection_t.STRAIGHT;

#if MOTOR_INIT_NEEDED
    escHasBeenInitialized = false;
#endif
}

MotorControl::MotorControl(){

}

#if MOTOR_INIT_NEEDED
void MotorControl::initCarMotor(){
    puts("Set throttle to neutral\n");
    MotorPwm.set(15);
    delay_ms(2000);

    puts("------Initializing PWM/ESC-------");
    puts("Will begin count down for programming ESC!\n");

    // Delay for 3 seconds to decide on setting up the ESC
    int countDown = 3;
    while (countDown > 0){
        puts("Ready to program ESC\n\n");

        if (escHasBeenInitialized == false && SW.getSwitch(4) == true){
            puts("----ESC PROGRAMMING MODE-----\n\n");
            LE.on(led4);
            do {
                puts("Checking for programming switches\n");
                if (SW.getSwitch(1) == 1)
                {
                    puts("\nSwitch 1 pressed, setting forward throttle\n");
                    LE.on(led1);

                    MotorPwm.set(15);
                    delay_ms(500);
                    MotorPwm.set(16);
                    delay_ms(500);
                    MotorPwm.set(17);
                    delay_ms(500);
                    MotorPwm.set(18);
                    delay_ms(500);
                    MotorPwm.set(19);
                    delay_ms(500);
                    MotorPwm.set(20);
                    delay_ms(500);

                    LE.off(led1);
                }

                //delay_ms(2000);

                if (SW.getSwitch(2) == 1)
                {
                    puts("\nSwitch 2 pressed, setting reverse throttle\n");
                    LE.on(led2);

                    MotorPwm.set(15);
                    delay_ms(500);
                    MotorPwm.set(14);
                    delay_ms(500);
                    MotorPwm.set(13);
                    delay_ms(500);
                    MotorPwm.set(12);
                    delay_ms(500);
                    MotorPwm.set(11);
                    delay_ms(500);
                    MotorPwm.set(10);
                    delay_ms(500);

                    MotorPwm.set(15);
                    delay_ms(2000);

                    LE.off(led2);

                    puts("\nESC has been programmed\n!");
                    escHasBeenInitialized = true;
                }
            } while(escHasBeenInitialized == false);
            puts("---------EXITING ESC PROGRAMMING MODE------\nn");
            LE.off(led4);  // End programming ESC mode

//            if(escHasBeenInitialized){
//                            while (1)
//                                {
//                                delay_ms(1000);
//                                motorPwm.set(15);
//                                    delay_ms(500);
//
//                                    motorPwm.set(15.5);
//                                    delay_ms(1000);
//
//                                    motorPwm.set(16);
//                                    delay_ms(1500);
//
//                                    motorPwm.set(16.5);
//                                    delay_ms(1500);
//
//                                    motorPwm.set(16);
//                                    delay_ms(1500);
//
//                                    motorPwm.set(15.5);
//                                    delay_ms(1000);
//
//                                    delay_ms(500);
//
//                                    motorPwm.set(14.8);
//                                    delay_ms(500);
//
//                                    motorPwm.set(14.5);
//                                    delay_ms(1000);
//
//                                    motorPwm.set(14);
//                                    delay_ms(1500);
//
//                                    motorPwm.set(13.5);
//                                    delay_ms(1500);
//
//                                    motorPwm.set(14);
//                                    delay_ms(1500);
//
//                                    motorPwm.set(14.5);
//                                    delay_ms(1000);
//
//                                    motorPwm.set(14.8);
//                                    delay_ms(500);
//
//                                    delay_ms(500);
//                                }
//            }
        }
        countDown--;
        delay_ms(1000);
    }

    puts("ESC assumed to be working!\n");
}
#endif

void MotorControl::setSteeringDirectionAndSpeed(float steeringDirectionToSet, float speedToSet){
    int pwmSetDelay = 30;

    // Set steering prior to changing motor speed
    CurrentServoValue = steeringDirectionToSet;
    ServoPwm.set(CurrentServoValue);

    if (speedToSet == speedSetting_t.BACK_SPEED){
        changeMotorDirection(speedToSet);
    }
    else{
        // If previously was moving going reverse, change speed to move forward
        if (CurrentMotorValue == speedSetting_t.BACK_SPEED){
            changeMotorDirection(speedToSet);
        }
        else{       // Normal operation
            CurrentMotorValue = speedToSet;
            MotorPwm.set(CurrentMotorValue);
            delay_ms(pwmSetDelay);
        }
    }
}

void MotorControl::changeMotorDirection(float speedToSet){
    // This is necessary in order for motor to be able to change from
    // forward to reverse, or reverse to forward
    MotorPwm.set(speedSetting_t.STOP);
    delay_ms(30);
    CurrentMotorValue = speedToSet;
    MotorPwm.set(CurrentMotorValue);
}

#if Motor_Control_Functions
void MotorControl::forward(float speedToSet){
    printf("-----Moving Forward--------\n");

    if (speedToSet == speedSetting_t.NO_CHANGE){
        setSteeringDirectionAndSpeed(steeringDirection_t.STRAIGHT, CurrentMotorValue);
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
            LE.on(led4);
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


void MotorControl::getCANMessageData(can_fullcan_msg_t *fullCanMessage, mast_mot_msg_t *motorControlStructToUse){
    if (fullCanMessage != NULL){
        //motorControlStruct->FRS = fc1->data.bytes[0];
        motorControlStructToUse->LR =  fullCanMessage->data.bytes[0];
        motorControlStructToUse->SPD = fullCanMessage->data.bytes[1];
        puts("--------Received data from CAN Message--------------\n");
    }
}

void MotorControl::convertFromHexAndApplyMotorAndServoSettings(mast_mot_msg_t *hexMotorControl){
    setSteeringDirectionAndSpeed(convertHexToFloatSteer(hexMotorControl->LR), convertHexToFloatSpeed(hexMotorControl->SPD));
#if 0
    if (hexMotorControl->FRS == 0x11){
        puts("====STOPPING MOTORS====\n");
        CurrentMotorValue = speedSetting_t.STOP;
        CurrentServoValue = steeringDirection_t.STRAIGHT;
        setSteeringDirectionAndSpeed(CurrentServoValue,CurrentMotorValue);
    }
    else if (hexMotorControl->FRS == 0xFF){
        if (hexMotorControl->SPD == 0x11){
            puts("Sending slow speed\n");
            CurrentMotorValue = speedSetting_t.SLOW_SPEED;
            if (hexMotorControl->LR == 0x00)
        }
        else if (hexMotorControl->SPD == 0x80){
            puts("Sending medium speed\n");
            CurrentMotorValue = speedSetting_t.MEDIUM_SPEED;
            forward(CurrentMotorValue);
        }
        else if (hexMotorControl->SPD == 0xFF){
            puts("Sending fast speed\n");
            CurrentMotorValue = speedSetting_t.FAST_SPEED;
            forward(CurrentMotorValue);
        }
        else{
            puts("Setting default speed to slow\n");
            CurrentMotorValue = speedSetting_t.SLOW_SPEED;
            forward(CurrentMotorValue);
        }
    }
#endif
}

float MotorControl::convertHexToFloatSpeed(uint16_t hexSpeedValue){
    float convertedHexToFloat = 0;
    
    if (hexSpeedValue == 0xFF)
        convertedHexToFloat = speedSetting_t.FAST_SPEED;
    if (hexSpeedValue == 0x80)
        convertedHexToFloat = speedSetting_t.MEDIUM_SPEED;
    if (hexSpeedValue == 0x11)
        convertedHexToFloat = speedSetting_t.SLOW_SPEED;
    if (hexSpeedValue == 0x02)
        convertedHexToFloat = speedSetting_t.BACK_SPEED;
    if (hexSpeedValue == 0x00)
        convertedHexToFloat = speedSetting_t.STOP;

    return convertedHexToFloat;
}

float MotorControl::convertHexToFloatSteer(uint16_t hexSteerValue){
    float convertedHexToFloat = 0;

    if (hexSteerValue == 0x80)
        convertedHexToFloat = steeringDirection_t.STRAIGHT;
    if (hexSteerValue == 0x00)
        convertedHexToFloat = steeringDirection_t.FULL_LEFT;
    if (hexSteerValue == 0xFF)
        convertedHexToFloat = steeringDirection_t.FULL_RIGHT;

    return convertedHexToFloat;
}
