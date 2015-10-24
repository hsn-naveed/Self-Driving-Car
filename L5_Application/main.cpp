#include "tasks.hpp"
#include <stdio.h>
#include "eint.h"
#include "io.hpp"
#include "can.h"
#include "uart2.hpp"

/*void initCAN (void)
{
    uint32_t baud_kbps = 100;
    uint16_t rxq_size = 16;
    uint16_t txq_size = 16;

     Initialization
    CAN_init(can1, baud_kbps, rxq_size, txq_size, NULL, NULL);
    CAN_bypass_filter_accept_all_msgs();
    CAN_reset_bus(can1);
}

class droid_rx : public scheduler_task
{
    public:
        droid_rx(uint8_t priority) :
            scheduler_task("droid_rx", 512, priority)
        {

        }

        bool init(void)
        {
            CAN_init(can1, 100, 16, 16, NULL, NULL);
            CAN_bypass_filter_accept_all_msgs();
            CAN_reset_bus(can1);
        }

        bool run(void *p)
        {
            can_msg_t msg_rx = {0};   // INITIALIZE

            printf("RECIEVING\n");
            if(CAN_rx(can1, &msg_rx, 100))
            {
                printf("RECIEVED\n");

                printf("%x\n", msg_rx.msg_id);
                printf("%x\n", msg_rx.data.qword);
            }

            return true;
        }
};*/


int main(void)
{
    scheduler_add_task(new terminalTask(PRIORITY_HIGH));

//    scheduler_add_task(new droid_rx(PRIORITY_HIGH));

#if 1
    scheduler_add_task(new periodicSchedulerTask());
#endif

    scheduler_start(); ///< This shouldn't return

    return -1;
}
