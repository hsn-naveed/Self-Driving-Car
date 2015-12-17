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
    //initLCD();
    /*
     * init the LCD
     * send 0xF0 to baud detection
     * 1) Clear the screen
     * 2) Set cursor to the beginning or print out a welcome message and set the cursor to the end of that message
     */
}
void LCD::initLCD()
{
    int delay = 10;
    if (lcdHasBeenInitialized == false){
        /// Initialize the UART baudrate
        lcd.init(9600, 10, 10);
        delay_ms(delay);
        lcdHasBeenInitialized = true;
    }

    /// Send initializing characters for LCD
    char initChar = '0xF0';
    lcd.putChar(initChar, portMAX_DELAY);
    delay_ms(delay);

    /// Send initial LCD messages
    lcd.putline("$GOTO:0:0");
    lcd.putline("Undergrads++");
    delay_ms(delay);

    lcd.putline("$GOTO:0:1");
    lcd.putline("SPD:"); //speed value should be written to row 1, column 4
    delay_ms(delay);

    lcd.putline("$GOTO:9:1");
    lcd.putline("CUR: ");
    delay_ms(delay);

    lcd.putline("$GOTO:9:2");
    lcd.putline("DST: ");
    delay_ms(delay);

    lcd.putline("$GOTO:0:2");
    lcd.putline("STR:"); //steering value should be written to row 1, column 13
    delay_ms(delay);

    /// Set LCD brightness level
    currentLCDbrightness = HighLCDbrightness;
    lcd.putline("$BLIGHT:80");

    printf("LCD initialized\n");

}

void LCD::writeSpeedAndSteerToLCD(char *speedVal, char* steerVal)
{
    lcd.putline("$GOTO:5:1");
    lcd.putline(speedVal, portMAX_DELAY);
    lcd.putline("$GOTO:5:2");
    lcd.putline(steerVal, portMAX_DELAY);

}
void LCD::writeHeadingData(char *currentHeading, char*destHeading){
    lcd.putline("$GOTO:14:1");
   lcd.printf("%s", *currentHeading);

   lcd.putline("$GOTO:14:2");
   lcd.printf("$s", *destHeading);
}
char* LCD::convertIntToCharSpeed(uint8_t hexSpeedValue)
{
    char *convertedSpeedValue = 0;
    if (hexSpeedValue == (uint8_t) COMMAND_FAST)
        convertedSpeedValue = "FS";
//        //convertedSpeedValue = (char) motorObj.FAST_SPEED;
//        sprintf(convertedSpeedValue, "%f", motorObj.FAST_SPEED);
    if (hexSpeedValue == (uint8_t) COMMAND_MEDIUM)
        convertedSpeedValue = "MS";
//        //  convertedSpeedValue = (char) motorObj.MEDIUM_SPEED;
//        sprintf(convertedSpeedValue, "%f", motorObj.MEDIUM_SPEED);
    if (hexSpeedValue == (uint8_t) COMMAND_SLOW)
        convertedSpeedValue = "SS";
//        sprintf(convertedSpeedValue, "%f", motorObj.SLOW_SPEED);
    if (hexSpeedValue == (uint8_t) COMMAND_REVERSE)
        convertedSpeedValue = "RV";
        //convertedSpeedValue = (char) motorObj.BACK_SPEED;
//        sprintf(convertedSpeedValue, "%f", motorObj.BACK_SPEED);
    return convertedSpeedValue;
}
char* LCD::convertIntToCharSteer(uint8_t hexSteerValue)
{
    char *convertedSteerValue = 0;
    if (hexSteerValue == (uint8_t) COMMAND_STRAIGHT)
        convertedSteerValue = "ST";
//        // convertedSteerValue = (char)STRAIGHT_LCD;
//        sprintf(convertedSteerValue, "%f", motorObj.STRAIGHT);
    if (hexSteerValue == (uint8_t) COMMAND_HARD_LEFT)
        convertedSteerValue = "HL";
        // convertedSteerValue = (char)FULL_LEFT_LCD;
//        sprintf(convertedSteerValue, "%f", motorObj.FULL_LEFT);
    if (hexSteerValue == (uint8_t) COMMAND_SOFT_LEFT)
        convertedSteerValue = "SL";
//        sprintf(convertedSteerValue, "%f", motorObj.SOFT_LEFT);
    if (hexSteerValue == (uint8_t) COMMAND_HARD_RIGHT)
        convertedSteerValue = "HR";
        // convertedSteerValue = (char)FULL_RIGHT_LCD;
//        sprintf(convertedSteerValue, "%f", motorObj.FULL_RIGHT);
    if (hexSteerValue == (uint8_t) COMMAND_SOFT_RIGHT)
        convertedSteerValue = "SR";
//        sprintf(convertedSteerValue, "%f", motorObj.SOFT_RIGHT);
    return convertedSteerValue;
}

void LCD::getMessageDataForMotor(MASTER_TX_MOTOR_CMD_t *DataforLCD_motorControlStruct)
{
    if (DataforLCD_motorControlStruct != NULL)
    {
        uint8_t steeringIntVal = (uint8_t) DataforLCD_motorControlStruct->MASTER_MOTOR_CMD_steer;
        uint8_t speedIntVal = (uint8_t) DataforLCD_motorControlStruct->MASTER_MOTOR_CMD_drive;

        writeSpeedAndSteerToLCD(convertIntToCharSpeed(speedIntVal),
                convertIntToCharSteer(steeringIntVal));
    }
}

void LCD::getMessageDataForGPS(GPS_TX_INFO_HEADING_t *gpsHeadingInfo){
    if (gpsHeadingInfo != NULL){
        int32_t currentHeading = gpsHeadingInfo->GPS_INFO_HEADING_current;
        int32_t destinationHeading = gpsHeadingInfo->GPS_INFO_HEADING_dst;

        char * curHead;
        char * destHead;

        sprintf(curHead, "%i", currentHeading);
        sprintf(destHead, "%i", destinationHeading);

        writeHeadingData(curHead, destHead);
    }
}

void LCD::clearLCD()
{
    lcd.putline("$CLR_SCR");
}

void LCD::moveCursor(int row, int column)
{
    //char *gotoCommand = "$GOTO"
//lcd.putline("");
}
void LCD::toggleLCDBrightness(){
    if(currentLCDbrightness == LowLCDbrightness){
        currentLCDbrightness = HighLCDbrightness;
        lcd.putline("$BLIGHT:80");
    }
    else if (currentLCDbrightness == HighLCDbrightness){
        currentLCDbrightness = LowLCDbrightness;
        lcd.putline("$BLIGHT:20");
    }
}
