//#include "LCD.hpp"
//
///* Pinout:-
// * VSS  -  ground
// * VDD  -  +5V
// * V0   -  adjust contrast
// * RS   -  register select for sending commands or sending characters i.e: clear display, turn off display When set
//
//to ground - send command,
// *         when high, send data
// * R/W  -  read / write (send commands here , read to check for a busy flag) If pin D7 is high then the LCD is
//
//busy,
// *         to check D7 the RW pin must be high
// * E    -  "Light switch of the LCD" allows the LCD to see the state of other pins. When flashed high and then low
//
//again,
// *         the LCD can see the state of the other pins so it can execute the proper instructions
// * D0   -  D0 - D7: 8 bit parrallel data port
// * D1   -
// * D2   -
// * D3   -
// * D4   -
// * D5   -
// * D6   -
// * D7   -
// * LEDA -  LED backlight anode
// * LEDK -  LED backlight cathode
// *
// *
// *
// */
//
//LCD::LCD(uint8_t priorityToUse)
//{
//    /*
//     * init the LCD
//     * 1) Clear the screen
//     * 2) Set cursor to the beginning or print out a welcome message and set the cursor to the end of that message
//     */
//}
//bool LCD::run(void *p)
//{
//
//}
//void LCD::writetoLCD(string datatoDisplay)
//{
//    string data;
//    data = datatoDisplay;
//
//   // if ()
//    { //if LCD is not busy *will edit later
//
//    }
//
//}
//void LCD::clearLCD()
//{
//
//}
//void LCD::moveCursor(int row, int column)
//{
//
//}
