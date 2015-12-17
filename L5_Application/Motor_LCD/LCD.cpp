/*
 * LCD.cpp
 *
 * Created by Jashan and Hector
 *
 *
 */
#include "LCD.hpp"
#include "utilities.h"
#include "stdio.h"
#include "uart3.hpp"
#include "243_can/CAN_structs.h"


/* Pinout:-
 * VSS  -  ground
 * VDD  -  +5V
 * V0   -  adjust contrast
 * RS   -  register select for sending commands or sending characters i.e: clear display, turn off display.
 *          When set to ground - send command,
 *          when high, send data
 * R/W  -  read / write (send commands here , read to check for a busy flag) If pin D7 is high then the LCD is

 busy,
 *         to check D7 the RW pin must be high
 * E    -  "Light switch of the LCD" allows the LCD to see the state of other pins. When flashed high and then low

 again,
 *         the LCD can see the state of the other pins so it can execute the proper instructions
 * D0   -  D0 - D7: 8 bit parrallel data port
 * D1   -
 * D2   -
 * D3   -
 * D4   -
 * D5   -
 * D6   -
 * D7   -
 * LEDA -  LED backlight anode
 * LEDK -  LED backlight cathode
 *
 *
 *
 */

LCD::LCD()
{
    initLCD();
    /*
     * init the LCD
     * send 0xF0 to baud detection
     * 1) Clear the screen
     * 2) Set cursor to the beginning or print out a welcome message and set the cursor to the end of that message
     */
}
void LCD::initLCD()
{
    lcd.init(9600, 10, 10);
    delay_ms(10);
    char initChar = '0xF0';
    lcd.putChar(initChar, portMAX_DELAY);
    delay_ms(10);
    initVariables();
    lcd.putline("$GOTO:0:0");
    lcd.putline("Undergrads++");
    delay_ms(10);
    lcd.putline("$GOTO:1:0");
    lcd.putline("SPD:"); //speed value should be written to row 1, column 4
    delay_ms(10);
    lcd.putline("$GOTO:1:7");
    lcd.putline("STR:"); //steering value should be written to row 1, column 13
    delay_ms(10);
    printf("LCD initialized\n");

}
void LCD::initVariables()
{
    sprintf(FULL_LEFT_LCD, "%f", motorObj.FULL_LEFT);
    sprintf(STRAIGHT_LCD, "%f", motorObj.STRAIGHT);
    sprintf(FULL_RIGHT_LCD, "%f", motorObj.FULL_RIGHT);
}
void LCD::writetoLCD(char *speedVal, char* steerVal)
{
    lcd.putline("$GOTO:1:4");
    lcd.putline(speedVal, portMAX_DELAY);
    lcd.putline("$GOTO:1:13");
    lcd.putline(steerVal, portMAX_DELAY);

}
char* LCD::convertHextoCharSpeed(uint8_t hexSpeedValue)
{
    char *convertedSpeedValue = 0;
    if (hexSpeedValue == (uint8_t) COMMAND_FAST)
        //convertedSpeedValue = (char) motorObj.FAST_SPEED;
        sprintf(convertedSpeedValue, "%f", motorObj.FAST_SPEED);
    if (hexSpeedValue == (uint8_t) COMMAND_MEDIUM)
        //  convertedSpeedValue = (char) motorObj.MEDIUM_SPEED;
        sprintf(convertedSpeedValue, "%f", motorObj.MEDIUM_SPEED);
    if (hexSpeedValue == (uint8_t) COMMAND_REVERSE)
        //convertedSpeedValue = (char) motorObj.BACK_SPEED;
        sprintf(convertedSpeedValue, "%f", motorObj.BACK_SPEED);
    return convertedSpeedValue;
}
char* LCD::convertHextoCharSteer(uint8_t hexSteerValue)
{
    char *convertedSteerValue = 0;
    if (hexSteerValue == (uint8_t) COMMAND_STRAIGHT)
        // convertedSteerValue = (char)STRAIGHT_LCD;
        sprintf(convertedSteerValue, "%f", motorObj.STRAIGHT);
    if (hexSteerValue == (uint8_t) COMMAND_HARD_LEFT)
        // convertedSteerValue = (char)FULL_LEFT_LCD;
        sprintf(convertedSteerValue, "%f", motorObj.FULL_LEFT);
    if (hexSteerValue == (uint8_t) COMMAND_SOFT_LEFT)
        sprintf(convertedSteerValue, "%f", motorObj.SOFT_LEFT);
    if (hexSteerValue == (uint8_t) COMMAND_HARD_RIGHT)
        // convertedSteerValue = (char)FULL_RIGHT_LCD;
        sprintf(convertedSteerValue, "%f", motorObj.FULL_RIGHT);
    if (hexSteerValue == (uint8_t) COMMAND_SOFT_RIGHT)
        sprintf(convertedSteerValue, "%f", motorObj.SOFT_RIGHT);
    return convertedSteerValue;

}
void LCD::getMessageDataFromMotor(mast_mot_msg_t *DataforLCD_motorControlStruct)
{
    {
        if (DataforLCD_motorControlStruct != NULL)
        {
            uint8_t steeringVal = (uint8_t) DataforLCD_motorControlStruct->LR;
            uint8_t speedVal = (uint8_t) DataforLCD_motorControlStruct->SPD;
            writetoLCD(convertHextoCharSpeed(speedVal),
                    convertHextoCharSteer(steeringVal));
        }
    }
}
void LCD::clearLCD()
{

}
void LCD::moveCursor(int row, int column)
{
    //char *gotoCommand = "$GOTO"
//lcd.putline("");
}

