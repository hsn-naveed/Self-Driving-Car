/*
 * CANBus.hpp
 */

#include <stdio.h>
#include "tasks.hpp"
#include "can.h"
#include "shared_handles.h"
#include "CAN_structs.h"

class CANBus :public scheduler_task{
    private:
        can_t canBusObj;
        uint32_t canBusBaudRate_Kbps = 100;
        uint16_t canBusRxQueueSize = 50;
        uint16_t canBusTxQueueSize = 50;
    public:
        CANBus(uint8_t priorityToUse);

        bool init(void);
        bool run(void *p);
};
