/*
 * communication.hpp
 *
 *  Created on: Nov 17, 2015
 *      Author: mac
 */

#ifndef L5_APPLICATION_COMMUNICATION_HPP_
#define L5_APPLICATION_COMMUNICATION_HPP_

#include "tasks.hpp"
#include "uart2.hpp"



class bluetoothTask : public scheduler_task
{
    public:
        bluetoothTask(uint8_t priority) :
            scheduler_task("bluetooth", 2048, priority)
        {
            /* Nothing to init */
        }
        bool run(void *p);

    private:
        Uart2 &bluetoothUart = Uart2::getInstance();
        char input;
};




#endif /* L5_APPLICATION_COMMUNICATION_HPP_ */
