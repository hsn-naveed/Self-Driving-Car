/*
 * communication.cpp
 *
 *  Created on: Nov 17, 2015
 *      Author: mac
 */

#include "communication.hpp"


bool bluetoothRecieve::run(void *p)
{
    char input;
    if (bluetoothUart.getChar(&input, 100))
    {
        bluetooth_list.push_back(input);
    }
    float x1;

    if (bluetooth_list.size() == 4)
    {
        x1 = bluetooth_list[0] << 0 | bluetooth_list[1] << 8 | bluetooth_list[2] << 16 | bluetooth_list[3] << 24;
        printf("%f \n", x1);
    }

    /*char tempChar;
    bluetoothUart.getChar(&tempChar, portMAX_DELAY);
    input[inputCounter] = tempChar;
    inputCounter++;

    if (inputCounter >= 7){
        for (int i = 0; i < inputCounter; i++){
            printf("input[%i] = %c\n", inputCounter, (char)(input[i] & 0xFF));
        }
        inputCounter = 0;
    }
    else{
        for (int i = 0; i < inputCounter; i++)
        printf("input[%i] = %c\n", inputCounter, (char)(input[i] & 0xFF));
    }*/


//        if (strncmp(input, GO_Msg, sizeof(GO_Msg)) == 0)
//            printf("\n%s\n", input);
//        else if (strncmp(input, STOP_Msg, sizeof(STOP_Msg)) == 0)
//            printf("\n%s\n", input);
//        else if (strncmp(input, STOP_Msg, sizeof(BTS)) == 0 || strncmp(input, STOP_Msg, sizeof(CON)) == 0)
//            return true;
//        else
//        {
//            bluetooth_list.push_back(input);
//            printf("%c\n", bluetooth_list[0]);
//        }

    //printf("\n\nsize: %d\n\n", bluetooth_list.size());

    return true;
}
