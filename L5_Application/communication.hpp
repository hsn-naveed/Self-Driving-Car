/*
 * communication.hpp
 *
 *  Created on: Nov 17, 2015
 *      Author: mac
 */

#ifndef L5_APPLICATION_COMMUNICATION_HPP_
#define L5_APPLICATION_COMMUNICATION_HPP_

#include <stdio.h>
#include <string.h>
#include "tasks.hpp"
#include "uart2.hpp"
#include "scheduler_task.hpp"
#include "io.hpp"
#include "vector.hpp"

#define BTS         "+BTSTATE"
#define CON         "CONNECT"
#define GO_Msg      "GO"
#define STOP_Msg    "STOP"

class bluetoothRecieve : public scheduler_task
{
    public:
        bluetoothRecieve(uint8_t priority) : scheduler_task("Receieve", 2048, priority)
        {

        }
        bool run(void *p);

    private:
        Uart2 &bluetoothUart = Uart2::getInstance();
        int inputCounter = 0;
        VECTOR<char> bluetooth_list;
};

#endif /* L5_APPLICATION_COMMUNICATION_HPP_ */
