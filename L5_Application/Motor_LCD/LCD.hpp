/*
 * LCD.hpp
 *
 * Created by Jashan and Hector
 */

#define LCD_HPP_

#include "io.hpp"
#include "lpc_pwm.hpp"
#include "scheduler_task.hpp"
#include "FreeRTOS.h"
#include <string>
#include "stdlib.h"
#include "CAN_structs.h"
#include "can.h"
#include "uart3.hpp"

/* Pinout:-
 * VSS  -  ground
 * VDD  -  +5V
 * V0   -  adjust contrast
 * RS   -  register select for sending commands or sending characters i.e: clear display, turn off display When set

to ground - send command,
 *         when high, send data
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
 */
/*
 * LCD Commands
 * $CLR_SCR -> clears the screen
 *
 */

using namespace std;

class LCD
{
    public:
        LCD();
        bool LCD_busy();
        void getCANmessageData(can_fullcan_msg_t *fc1, mast_mot_msg_t *motorControlStruct);
        void writetoLCD(char *datatoDisplay); //takes data and displays on the LCD
        void clearLCD(); //clears the entire LCD
        void moveCursor(int row, int column); //moves the cursor to a desired point

        void initLCD();
    private:
        Uart3& lcd = Uart3::getInstance();

};
