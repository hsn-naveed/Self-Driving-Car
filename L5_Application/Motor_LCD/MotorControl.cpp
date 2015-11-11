/*
 * MotorControl.cpp
 *
 * Created by Jashan and Hector
 *
 *
 */

#include "MotorControl.hpp"

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
    MotorPwm.set(speedSetting_t.STOP);
    delay_ms(2000);

    puts("------Initializing PWM/ESC-------");
    puts("Will begin count down for programming ESC!\n");

    // Delay for 3 seconds to decide on setting up the ESC
    int countDown = 3;
    int delay = 500;
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
                    delay_ms(delay);
                    MotorPwm.set(16);
                    delay_ms(delay);
                    MotorPwm.set(17);
                    delay_ms(delay);
                    MotorPwm.set(18);
                    delay_ms(delay);
                    MotorPwm.set(19);
                    delay_ms(delay);
                    MotorPwm.set(20);

                    LE.off(led1);
                    delay_ms(2000);
                }

                if (SW.getSwitch(2) == 1)
                {
                    puts("\nSwitch 2 pressed, setting reverse throttle\n");
                    LE.on(led2);
                    delay_ms(delay);

                    MotorPwm.set(15);
                    delay_ms(1500);
                    MotorPwm.set(14);
                    delay_ms(delay);
                    MotorPwm.set(13);
                    delay_ms(delay);
                    MotorPwm.set(12);
                    delay_ms(delay);
                    MotorPwm.set(11);
                    delay_ms(delay);
                    MotorPwm.set(10);
                    delay_ms(delay);

                    LE.off(led2);

                    delay_ms(2000);

                    puts("\nESC has been programmed\n!");
                    escHasBeenInitialized = true;
                }
            } while(escHasBeenInitialized == false);
            puts("---------EXITING ESC PROGRAMMING MODE------\nn");
            LE.off(led4);  // End programming ESC mode
            MotorPwm.set(15);
            /*if(escHasBeenInitialized){
                            while (1)
                                {
                                delay_ms(1000);
                                MotorPwm.set(15);
                                    delay_ms(500);

                                    MotorPwm.set(15.5);
                                    delay_ms(1000);

                                    MotorPwm.set(16);
                                    delay_ms(1500);

                                    MotorPwm.set(16.5);
                                    delay_ms(1500);

                                    MotorPwm.set(16);
                                    delay_ms(1500);

                                    MotorPwm.set(15.5);
                                    delay_ms(1000);

                                    delay_ms(500);

                                    MotorPwm.set(14.8);
                                    delay_ms(500);

                                    MotorPwm.set(14.5);
                                    delay_ms(1000);

                                    MotorPwm.set(14);
                                    delay_ms(1500);

                                    MotorPwm.set(13.5);
                                    delay_ms(1500);

                                    MotorPwm.set(14);
                                    delay_ms(1500);

                                    MotorPwm.set(14.5);
                                    delay_ms(1000);

                                    MotorPwm.set(14.8);
                                    delay_ms(500);

                                    delay_ms(500);
                                }
            }*/
        }
        countDown--;
        delay_ms(1000);
    }

    puts("ESC assumed to be working!\n");
}
#endif

void MotorControl::setSteeringDirectionAndSpeed(float steeringDirectionToSet, float speedToSet){
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

        }
    }
}

void MotorControl::changeMotorDirection(float speedToSet){
    // This is necessary in order for motor to be able to change from
    // forward to reverse, or reverse to forward
    MotorPwm.set(speedSetting_t.BRAKE);
    CurrentMotorValue = speedToSet;
    MotorPwm.set(CurrentMotorValue);
}
#endif // Public Functions


void MotorControl::getCANMessageData(can_fullcan_msg_t *fullCanMessage, mast_mot_msg_t *motorControlStructToUse){
    if (fullCanMessage != NULL){
        motorControlStructToUse->LR =  (uint8_t)fullCanMessage->data.bytes[0];
        motorControlStructToUse->SPD = (uint8_t) fullCanMessage->data.bytes[1];
    }
}

void MotorControl::convertFromHexAndApplyMotorAndServoSettings(mast_mot_msg_t *hexMotorControl){
    if (hexMotorControl != NULL){
        uint8_t steeringHexVal = (uint8_t) hexMotorControl->LR;
        uint8_t speedHexVal = (uint8_t) hexMotorControl->SPD;

        setSteeringDirectionAndSpeed(convertHexToFloatSteer(steeringHexVal), convertHexToFloatSpeed(speedHexVal));
    }
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

float MotorControl::convertHexToFloatSpeed(uint8_t hexSpeedValue){
    float convertedHexToFloat = 0;
    
    if (hexSpeedValue == (uint8_t)COMMAND_FAST)
        convertedHexToFloat = speedSetting_t.FAST_SPEED;
    if (hexSpeedValue == (uint8_t)COMMAND_MEDIUM)
        convertedHexToFloat = speedSetting_t.MEDIUM_SPEED;
    if (hexSpeedValue == (uint8_t)COMMAND_SLOW)
        convertedHexToFloat = speedSetting_t.SLOW_SPEED;
    if (hexSpeedValue == (uint8_t)COMMAND_REVERSE)
        convertedHexToFloat = speedSetting_t.BACK_SPEED;
    if (hexSpeedValue == (uint8_t)COMMAND_STOP)
        convertedHexToFloat = speedSetting_t.BRAKE;

    return convertedHexToFloat;
}

float MotorControl::convertHexToFloatSteer(uint8_t hexSteerValue){
    float convertedHexToFloat = 0;

    if (hexSteerValue == (uint8_t)COMMAND_STRAIGHT)
        convertedHexToFloat = (float)steeringDirection_t.STRAIGHT;
    if (hexSteerValue == (uint8_t)COMMAND_LEFT)
        convertedHexToFloat = (float)steeringDirection_t.FULL_LEFT;
    if (hexSteerValue == (uint8_t)COMMAND_RIGHT)
        convertedHexToFloat = (float)steeringDirection_t.FULL_RIGHT;

    return convertedHexToFloat;
}
