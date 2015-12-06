#include "tasks.hpp"
#include "communication.hpp"
int main(void)
{
    scheduler_add_task(new terminalTask(PRIORITY_CRITICAL));

    scheduler_add_task(new periodicSchedulerTask());

    scheduler_start();

    return -1;
}
