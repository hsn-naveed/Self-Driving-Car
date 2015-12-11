#include "tasks.hpp"

int main(void)
{

    scheduler_add_task(new terminalTask(PRIORITY_CRITICAL));

    scheduler_add_task(new periodicSchedulerTask());

    scheduler_start();

    return -1;
}
