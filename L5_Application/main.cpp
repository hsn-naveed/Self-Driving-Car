#include "tasks.hpp"
#include <stdio.h>
#include "io.hpp"
#include "communication.hpp"
#include "243_can/iCAN.hpp"
#include <vector.hpp>

int main(void)
{
    scheduler_add_task(new terminalTask(PRIORITY_HIGH));
//    scheduler_add_task(new bluetoothRecieve(PRIORITY_MEDIUM));

    scheduler_add_task(new periodicSchedulerTask());

    scheduler_start();

    return -1;
}
