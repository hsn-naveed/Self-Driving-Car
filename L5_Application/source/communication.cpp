/*
 * communication.cpp
 *
 *  Created on: Nov 17, 2015
 *      Author: mac
 */

#include "tasks.hpp"
#include "communication.hpp"
#include <stdio.h>
#include "io.hpp"



bool bluetoothTask::run(void *p)
{


    if (bluetoothUart.getChar(&input, 100))
    {
        putchar(input);
    }


    if (SW.getSwitch(1))
    {
        bluetoothUart.putChar('s');
        printf("Sending.....");
    }

    return true;
}
