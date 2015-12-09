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
        for (int i = NEUTRAL; i <= maxOrMin; i += incrementAndDecrementSize){
            MotorPwm.set(i);
            delay_ms(pwmDelay);
        }
    }
    else{
        for (int i = NEUTRAL; i >= maxOrMin; i += incrementAndDecrementSize){
            MotorPwm.set(i);
            delay_ms(pwmDelay);
        }
    }
}

void MotorControl::changeMotorDirection(float speedToSet){
//    if (CurrentMotorValue == BACK_SPEED){
//        MotorPwm.set(FAST_SPEED);
//        delay_us(850);
//    }
if (CurrentMotorValue == BACK_SPEED){
        MotorPwm.set(FAST_SPEED);
        printf("Setting to fast speed to brake\n");
        delay_ms(500);
    }
else if (CurrentMotorValue > NEUTRAL){
    printf("Setting brake to stop");
    MotorPwm.set(BRAKE);
    delay_us(500);
}
    MotorPwm.set(NEUTRAL);
    delay_ms(500);

    CurrentMotorValue = speedToSet;
    printf("motor value in change direction = %.5f\n", CurrentMotorValue);
    MotorPwm.set(CurrentMotorValue);
}
#endif


#if 1   /// Public Functions
MotorControl::MotorControl(PWM &motorPwmToSet, PWM &servoPwmToSet){
    MotorPwm = motorPwmToSet;
    ServoPwm = servoPwmToSet;

    CurrentMotorValue = NEUTRAL;
    CurrentServoValue = STRAIGHT;

    escHasBeenInitialized = false;
}

MotorControl::MotorControl(){

}

void MotorControl::initESC(){
    puts("Setting throttle to neutral\n");
    MotorPwm.set(NEUTRAL);
    delay_ms(500);

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
            MotorPwm.set(NEUTRAL);
        }
        countDown--;
        delay_ms(1000);
    }

    puts("ESC assumed to be working!\n");
    puts("Press switch 4 to begin autonomous mode!\n");
}

void MotorControl::pulseBrake()
{
    MotorPwm.set(BRAKE);
    MotorPwm.set(NEUTRAL);

    MotorPwm.set(BACK_SPEED);
//    MotorPwm.set(NEUTRAL);
}

void MotorControl::setSteeringDirectionAndSpeed(float steeringDirectionToSet, float speedToSet){
    // Set steering prior to changing motor speed
    CurrentServoValue = steeringDirectionToSet;
    ServoPwm.set(CurrentServoValue);

    if (speedToSet == BACK_SPEED){
        if ((CurrentMotorValue == BRAKE) || (CurrentMotorValue > NEUTRAL))
            changeMotorDirection(BACK_SPEED);
        else{
            CurrentMotorValue = speedToSet;
            MotorPwm.set(CurrentMotorValue);
        }
    }
    else if (speedToSet == BRAKE){
        if (CurrentMotorValue == NEUTRAL){
            CurrentMotorValue = NEUTRAL;
            MotorPwm.set(NEUTRAL);
        }
        else {
            CurrentMotorValue = speedToSet;
            MotorPwm.set(CurrentMotorValue);
        }
    }
    else{
        if (CurrentMotorValue < NEUTRAL){
            changeMotorDirection(speedToSet);
        }else{
            CurrentMotorValue = speedToSet;
//            printf("got here\n");
            MotorPwm.set(CurrentMotorValue);
        }
    }
//    delay_ms(50);

//    if (speedToSet == BACK_SPEED){
//        //        if ((CurrentMotorValue != BACK_SPEED) && (CurrentMotorValue != NEUTRAL)){
//        //            changeMotorDirection(speedToSet);
//        //        }
//        MotorPwm.set(NEUTRAL);
//        delay_ms(1);
//
//        CurrentMotorValue = speedToSet;
//        printf("==Current motor value = %.5f\n", CurrentMotorValue);
//        MotorPwm.set(CurrentMotorValue);
//    }
//    else if ((speedToSet == BRAKE) &&
//            ((CurrentMotorValue == NEUTRAL) || CurrentMotorValue == BACK_SPEED)){
//        CurrentMotorValue = NEUTRAL;
//        MotorPwm.set(NEUTRAL);
//    }
//    else{
//        // If previously was moving going reverse, change speed to move forward
////        if (CurrentMotorValue == BACK_SPEED && (speedToSet == SLOW_SPEED || speedToSet == MEDIUM_SPEED
////                || speedToSet == FAST_SPEED)){
////            // FIX THIS
////            //changeMotorDirection(speedToSet);
////            CurrentMotorValue = speedToSet;
////                        printf("==Current motor value = %.5f\n", CurrentMotorValue);
////                        MotorPwm.set(CurrentMotorValue);
////        }
////        else{ // Normal operation
//            CurrentMotorValue = speedToSet;
//            printf("==Current motor value = %.5f\n", CurrentMotorValue);
//            MotorPwm.set(CurrentMotorValue);
////        }
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
    if (hexSteerValue == (uint8_t)COMMAND_FULL_LEFT)
        convertedHexToFloat = (float)FULL_LEFT;
    if (hexSteerValue == (uint8_t)COMMAND_SOFT_LEFT)
        convertedHexToFloat = (float)SOFT_LEFT;
    if (hexSteerValue == (uint8_t)COMMAND_FULL_RIGHT)
        convertedHexToFloat = (float)FULL_RIGHT;
    if (hexSteerValue == (uint8_t)COMMAND_SOFT_RIGHT)
        convertedHexToFloat = (float)SOFT_RIGHT;

    return convertedHexToFloat;
}
#endif // Public Functions
