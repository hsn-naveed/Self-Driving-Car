#include "tasks.hpp"
#include <stdio.h>
#include "io.hpp"
#include "can.h"
#include "communication.hpp"


int main(void)
{
    scheduler_add_task(new bluetoothTask(PRIORITY_HIGH));

    scheduler_add_task(new terminalTask(PRIORITY_HIGH));

#if 0
    scheduler_add_task(new periodicSchedulerTask());
#endif

    scheduler_start();

    return -1;
}
