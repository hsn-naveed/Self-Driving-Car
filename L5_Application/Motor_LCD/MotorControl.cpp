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
    /// Enter here if wanting to brake, going either forward or reverse
    if (speedToSet == BRAKE){
        /// If currently moving forward, just set pwm to BRAKE, as the BRAKE is
        // the full reverse, which is brake for the esc until you go back to neutral
        if (CurrentMotorValue == MEDIUM_SPEED || CurrentMotorValue == SLOW_SPEED){
            CurrentMotorValue = speedToSet;
            MotorPwm.set(CurrentMotorValue);
        }
        /// If moving backwards, and wanting to stop, send a FOWARD_BRAKE; this
        // is a full forward throttle, which will instantly stop the wheels, and won't move forward
        // until you return to neutral
        else if (CurrentMotorValue == BACK_SPEED){
            MotorPwm.set(FOWARD_BRAKE);
            delay_ms(10);
            CurrentMotorValue = speedToSet;
        }
    }
    /// Enter here if wanting to go reverse
    if (speedToSet == BACK_SPEED){
        if (CurrentMotorValue == MEDIUM_SPEED || CurrentMotorValue == SLOW_SPEED){
            MotorPwm.set(BRAKE);
            delay_ms(10);

            MotorPwm.set(NEUTRAL);
            delay_ms(10);

            CurrentMotorValue = speedToSet;
            MotorPwm.set(CurrentMotorValue);
        }
        /// If already stopped, and want to go reverse, you must first go to neutral,
        // then reverse; ESC will then know that the reverse is now reverse, instead of a brake
        else if (CurrentMotorValue == BRAKE){
            MotorPwm.set(NEUTRAL);
            delay_ms(10);

            CurrentMotorValue = speedToSet;
            MotorPwm.set(CurrentMotorValue);
        }
        else{   /// Currently in neutral, and want to reverse
            CurrentMotorValue = speedToSet;
            MotorPwm.set(CurrentMotorValue);
        }
    }
    if (speedToSet == MEDIUM_SPEED || speedToSet == SLOW_SPEED){
        if (CurrentMotorValue < NEUTRAL){
            MotorPwm.set(FOWARD_BRAKE);
            delay_ms(10);

            MotorPwm.set(NEUTRAL);
            delay_ms(10);

            CurrentMotorValue = speedToSet;
            MotorPwm.set(CurrentMotorValue);
        }
    }
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

    if (CurrentMotorValue == NEUTRAL){
        /// Used to prevent shooting forward or reverse in case "brakes" were recevied from CAN
        if (speedToSet == BACK_SPEED || speedToSet == SLOW_SPEED || speedToSet == MEDIUM_SPEED){
            CurrentMotorValue = speedToSet;
            MotorPwm.set(CurrentMotorValue);
        }
    }
    else{
        if(speedToSet != CurrentMotorValue)
            changeMotorDirection(speedToSet);
    }
}

void MotorControl::convertFromIntegerAndApplyServoAndMotorSettings(MASTER_TX_MOTOR_CMD_t *receviedMotorCommandsToUse){
    if (receviedMotorCommandsToUse != NULL){
        uint8_t steeringIntVal = (uint8_t) receviedMotorCommandsToUse->MASTER_MOTOR_CMD_steer;
        uint8_t speedIntVal = (uint8_t) receviedMotorCommandsToUse->MASTER_MOTOR_CMD_drive;

        setSteeringDirectionAndSpeed(convertIntegerToFloatSteer(steeringIntVal), convertIntegerToFloatSpeed(speedIntVal));
    }
}

/*@about Takes hex value passed in from mast_mot_msg_t in CAN_STRUCTS.h
 *          and converts to a float value (duty cycle %) for the PWM
 *          output
 *
 *@param hexSpeedValue - passed in hex value from mast_mot_msg_t
 *@output convertedHexToFloat - duty cycle % as float for PWM output
 */
float MotorControl::convertIntegerToFloatSpeed(uint8_t integerSpeedValue){
    float convertedIntegerToFloat = 0;

    if (integerSpeedValue == (uint8_t)COMMAND_FAST)
        convertedIntegerToFloat = FAST_SPEED;
    if (integerSpeedValue == (uint8_t)COMMAND_MEDIUM)
        convertedIntegerToFloat = MEDIUM_SPEED;
    if (integerSpeedValue == (uint8_t)COMMAND_SLOW)
        convertedIntegerToFloat = SLOW_SPEED;
    if (integerSpeedValue == (uint8_t)COMMAND_REVERSE)
        convertedIntegerToFloat = BACK_SPEED;
    if (integerSpeedValue == (uint8_t)COMMAND_STOP)
        convertedIntegerToFloat = BRAKE;

    return convertedIntegerToFloat;
}

float MotorControl::convertIntegerToFloatSteer(uint8_t integerSteerValue){
    float convertedIntegerToFloat = 0;

    if (integerSteerValue == (uint8_t)COMMAND_STRAIGHT)
        convertedIntegerToFloat = (float)STRAIGHT;
    if (integerSteerValue == (uint8_t)COMMAND_HARD_LEFT)
        convertedIntegerToFloat = (float)FULL_LEFT;
    if (integerSteerValue == (uint8_t)COMMAND_SOFT_LEFT)
        convertedIntegerToFloat = (float)SOFT_LEFT;
    if (integerSteerValue == (uint8_t)COMMAND_HARD_RIGHT)
        convertedIntegerToFloat = (float)FULL_RIGHT;
    if (integerSteerValue == (uint8_t)COMMAND_SOFT_RIGHT)
        convertedIntegerToFloat = (float)SOFT_RIGHT;

    return convertedIntegerToFloat;
}
#endif // Public Functions
