/*
 * MotorControl.cpp
 *
 * Created by Jashan and Hector
 *
 *
 */

#include "MotorControl.hpp"
// Just for regioning code
#define Private_Functions 1
#define Public_Functions 1
#define Motor_Control_Functions 1
#define Servo_Steering_Functions 1

#define Motor_LCD_controller_ID 0x3F2 //1010

#if Private_Functions
void MotorControl::setSteeringDirectionAndSpeed(float steeringDirectionToSet, float speedToSet){
    int pwmSetDelay = 30;
    currentServoValue = steeringDirectionToSet;
    currentMotorValue = speedToSet;

    motorPwm.set(currentMotorValue);
    servoPwm.set(currentServoValue);
    delay_ms(pwmSetDelay);
}
#endif // Private Functions

#if Public_Functions
MotorControl::MotorControl(PWM &motorPwmToSet, PWM &servoPwmToSet){
    puts("Before setting pwm\n");

    motorPwm = motorPwmToSet;
    servoPwm = servoPwmToSet;
    puts("after setting pwm\n");

    currentMotorValue = speedSetting_t.STOP;
    currentServoValue = steeringDirection_t.STRAIGHT;

    puts("before setting up motor struc\n");
//    *motor_control_struct = {0};
//    motor_control_struct->FRS = 0xFF;
//    motor_control_struct->LR = 0x80;
//    motor_control_struct->SPD = 0x11;

    puts("before setting up motor struc\n");

#if MOTOR_INIT_NEEDED
    escHasBeenInitialized = false;
#endif
}

MotorControl::MotorControl(){

}

#if MOTOR_INIT_NEEDED
void MotorControl::initCarMotor(){
    puts("Set throttle to neutral\n");
    motorPwm.set(15);
    delay_ms(2000);

    puts("------Initializing PWM/ESC-------");
    puts("Will begin count down for programming ESC!\n");

    // Delay for 3 seconds to decide on setting up the ESC
    int countDown = 3;
    while (countDown > 0){
        puts("Ready to program ESC\n\n");

        if (escHasBeenInitialized == false && SW.getSwitch(4) == true){
            puts("----ESC PROGRAMMING MODE-----\n\n");
            LE.on(4);
            do {
                puts("Checking for programming switches\n");
                if (SW.getSwitch(1) == 1)
                {
                    puts("\nSwitch 1 pressed, setting forward throttle\n");
                    LE.on(1);
//                    delay_ms(500);

                    motorPwm.set(15);
                    delay_ms(500);
                    motorPwm.set(16);
                    delay_ms(500);
                    motorPwm.set(17);
                    delay_ms(500);
                    motorPwm.set(18);
                    delay_ms(500);
                    motorPwm.set(19);
                    delay_ms(500);
                    motorPwm.set(20);
                    delay_ms(500);


                    LE.off(1);
                }

                //delay_ms(2000);

                if (SW.getSwitch(2) == 1)
                {
                    puts("\nSwitch 2 pressed, setting reverse throttle\n");
                    LE.on(2);
//                    delay_ms(500);

                    motorPwm.set(15);
                    delay_ms(500);
                    motorPwm.set(14);
                    delay_ms(500);
                    motorPwm.set(13);
                    delay_ms(500);
                    motorPwm.set(12);
                    delay_ms(500);
                    motorPwm.set(11);
                    delay_ms(500);
                    motorPwm.set(10);
                    delay_ms(500);

                    motorPwm.set(15);
                    delay_ms(2000);
                    LE.off(2);

                    puts("\nESC has been programmed\n!");
                    escHasBeenInitialized = true;
                }
            } while(escHasBeenInitialized == false);
            puts("---------EXITING ESC PROGRAMMING MODE------\nn");
            LE.off(4);  // End programming ESC mode

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


void MotorControl::getCANMessageData(can_msg_t *fc1, mast_mot_msg_t *motorControlStruct)
{
    if (fc1 != NULL){
        motorControlStruct->FRS = fc1->data.bytes[0];
        motorControlStruct->LR =  fc1->data.bytes[1];
        motorControlStruct->SPD = fc1->data.bytes[2];
        puts("--------Received data from CAN Message--------------\n");
    }
}

void MotorControl::convertHexToDutyCycle(mast_mot_msg_t *hexMotorControl){

    if (hexMotorControl->FRS == 0xFF){
        puts("In the convertHexToDutyCycle sending forward at slow speed\n");
        forward(speedSetting_t.SLOW_SPEED);
        currentMotorValue = speedSetting_t.SLOW_SPEED;
    }
}
