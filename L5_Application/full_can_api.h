/*
 * full_can_api.h
 *
 *  Created on: Nov 2, 2015
 *      Author: Hassan
 */

#ifndef L5_APPLICATION_FULL_CAN_API_H_
#define L5_APPLICATION_FULL_CAN_API_H_

#include "stdio.h"
#include "can.h"
#include "shared_handles.h"
#include "tasks.hpp"

class full_can_api //: public scheduler_task
{
    public:
        full_can_api();
        static bool bus_init();
        static bool can_rec(uint16_t id);
        static bool can_send(can_msg_t *msg, uint32_t timeout_ms);
        //bool run(void *p);
};



#endif /* L5_APPLICATION_FULL_CAN_API_H_ */
