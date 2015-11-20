/*
 * MotorControl.cpp
 *
 * Created by Jashan and Hector
 *
 *
 */

#include "MotorControl.hpp"

#if 1   /// Private Functions
void MotorControl::triggerForwardOrReverseThrottle(float maxOrMin,
                                                    double incrementAndDecrementSize,
                                                    int pwmDelay){
    /// Increment all the way to full throttle
    if (incrementAndDecrementSize > 0){
        for (int i = NEUTRAL; i < maxOrMin; i += incrementAndDecrementSize){
            MotorPwm.set(i);
            delay_ms(pwmDelay);
        }
    }
    else{
        for (int i = NEUTRAL; i > maxOrMin; i += incrementAndDecrementSize){
            MotorPwm.set(i);
            delay_ms(pwmDelay);
        }
    }
}

void MotorControl::changeMotorDirection(float speedToSet){
    // This is necessary in order for motor to be able to change from
    // forward to reverse, or reverse to forward
    MotorPwm.set(BRAKE);
    CurrentMotorValue = speedToSet;
    MotorPwm.set(CurrentMotorValue);
}
#endif


#if 1   /// Public Functions
void MotorControl::initAllGlobalsForMotorControl(){
    /// Used for PWM
    MOTOR_SERVO_PWM_FREQ = 100;

    /// For dynamically calculating
    ONE_SECOND_MS = 1000;
    NEUTRAL_PWM_PERIOD_MS = 1.5;
    FORWARD_PWM_PERIOD_MS = 2;
    REVERSE_PWM_PERIOD_MS = 1;

    /*
     * @about For precise tweaking of speed
     *
     * @warning Make sure to use negative for medium and slow
     * and positive for back speed, as it is probably more likely
     * that the speeds are too fast
     */
    MEDIUM_SPEED_OFFSET = 0;
    SLOW_SPEED_OFFSET = 0;
    BACK_SPEED_OFFSET = 0;
}

MotorControl::MotorControl(PWM &motorPwmToSet, PWM &servoPwmToSet){
    MotorPwm = motorPwmToSet;
    ServoPwm = servoPwmToSet;

    CurrentMotorValue = NEUTRAL;
    CurrentServoValue = STRAIGHT;

//    initAllGlobalsForMotorControl();
        escHasBeenInitialized = false;
}

MotorControl::MotorControl(){

}

void MotorControl::initESC(){
    puts("Setting throttle to neutral\n");
    MotorPwm.set(NEUTRAL);
    delay_ms(2000);

    puts("------Initializing PWM/ESC-------");
    puts("Will begin count down for programming ESC!\n");

    // Delay for 3 seconds to decide on setting up the ESC
    int countDown = 3;
    int pwmDelay = 500;
    double incrementAndDecrementSize = 1;
    while (countDown > 0){
        puts("Ready to program ESC\n\n");

        if (escHasBeenInitialized == false && SW.getSwitch(4) == true){
            puts("----ESC PROGRAMMING MODE-----\n\n");
            puts("Press switch1 for forward throttle\nPress switch2 for reverse throttle\n");
            LE.on(led4);

            do {
                puts("Checking for programming switches\n");
                if (SW.getSwitch(1) == 1)
                {
                    puts("\nSwitch 1 pressed, setting forward throttle\n");
                    LE.on(led1);

                    /// Increment all the way to full throttle
                    triggerForwardOrReverseThrottle(FAST_SPEED, incrementAndDecrementSize, pwmDelay);

                    LE.off(led1);
                    delay_ms(2000);
                }

                if (SW.getSwitch(2) == 1)
                {
                    puts("\nSwitch 2 pressed, setting reverse throttle\n");
                    LE.on(led2);

                    /// Multiply increment by negative to decrement
                    triggerForwardOrReverseThrottle(BRAKE,incrementAndDecrementSize*(-1), pwmDelay);

                    LE.off(led2);
                    delay_ms(2000);

                    puts("\nESC has been programmed\n!");
                    escHasBeenInitialized = true;
                }
            } while(escHasBeenInitialized == false);

            puts("---------EXITING ESC PROGRAMMING MODE------\n");
            LE.off(led4);  // End programming ESC mode

            // Finish off programming of ESC by returning to neutral
            MotorPwm.set(15);
        }
        countDown--;
        delay_ms(1000);
    }

    puts("ESC assumed to be working!\n");
}

void MotorControl::setSteeringDirectionAndSpeed(float steeringDirectionToSet, float speedToSet){
    // Set steering prior to changing motor speed
    CurrentServoValue = steeringDirectionToSet;
    ServoPwm.set(CurrentServoValue);

    if (speedToSet == BACK_SPEED){
        changeMotorDirection(speedToSet);
    }
    else{
        // If previously was moving going reverse, change speed to move forward
        if (CurrentMotorValue == BACK_SPEED){
            changeMotorDirection(speedToSet);
        }
        else{       // Normal operation
            CurrentMotorValue = speedToSet;
            MotorPwm.set(CurrentMotorValue);
        }
    }
}



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
}

float MotorControl::convertHexToFloatSpeed(uint8_t hexSpeedValue){
    float convertedHexToFloat = 0;

    if (hexSpeedValue == (uint8_t)COMMAND_FAST)
        convertedHexToFloat = FAST_SPEED;
    if (hexSpeedValue == (uint8_t)COMMAND_MEDIUM)
        convertedHexToFloat = MEDIUM_SPEED;
    if (hexSpeedValue == (uint8_t)COMMAND_SLOW)
        convertedHexToFloat = SLOW_SPEED;
    if (hexSpeedValue == (uint8_t)COMMAND_REVERSE)
        convertedHexToFloat = BACK_SPEED;
    if (hexSpeedValue == (uint8_t)COMMAND_STOP)
        convertedHexToFloat = BRAKE;

    return convertedHexToFloat;
}

float MotorControl::convertHexToFloatSteer(uint8_t hexSteerValue){
    float convertedHexToFloat = 0;

    if (hexSteerValue == (uint8_t)COMMAND_STRAIGHT)
        convertedHexToFloat = (float)STRAIGHT;
    if (hexSteerValue == (uint8_t)COMMAND_LEFT)
        convertedHexToFloat = (float)FULL_LEFT;
    if (hexSteerValue == (uint8_t)COMMAND_RIGHT)
        convertedHexToFloat = (float)FULL_RIGHT;

    return convertedHexToFloat;
}
#endif // Public Functions
