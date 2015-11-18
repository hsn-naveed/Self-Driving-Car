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



bluetoothTask::bluetoothTask(uint8_t priority) : scheduler_task("bluetooth", 2048, priority)
{

}

bool bluetoothTask::run(void *p)
{
    /*if (bluetoothUart.gets(input, sizeof(input), portMAX_DELAY))
    {
        LE.on(2);
    }*/

    if (SW.getSwitch(1))
    {
        LE.on(1);
        bluetoothUart.putChar('s');
        printf("Sending.....");
    }

    LE.off(1);
    return true;
}
