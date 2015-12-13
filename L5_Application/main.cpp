/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

/**
 * @file
 * @brief This is the application entry point.
 * 			FreeRTOS and stdio printf is pre-configured to use uart0_min.h before main() enters.
 * 			@see L0_LowLevel/lpc_sys.h if you wish to override printf/scanf functions.
 *
 */

#include <243_can/iCAN.hpp>
#include "tasks.hpp"
#include "examples/examples.hpp"
#include "io.hpp"
#include "stdio.h"
#include "file_logger.h"
#include "can.h"
#include <inttypes.h>



/**
 * The main() creates tasks or "threads".  See the documentation of scheduler_task class at scheduler_task.hpp
 * for details.  There is a very simple example towards the beginning of this class's declaration.
 *
 * @warning SPI #1 bus usage notes (interfaced to SD & Flash):
 *      - You can read/write files from multiple tasks because it automatically goes through SPI semaphore.
 *      - If you are going to use the SPI Bus in a FreeRTOS task, you need to use the API at L4_IO/fat/spi_sem.h
 *
 * @warning SPI #0 usage notes (Nordic wireless)
 *      - This bus is more tricky to use because if FreeRTOS is not running, the RIT interrupt may use the bus.
 *      - If FreeRTOS is running, then wireless task may use it.
 *        In either case, you should avoid using this bus or interfacing to external components because
 *        there is no semaphore configured for this bus and it should be used exclusively by nordic wireless.
 */
int main(void)
{

    /**
     * Initializes the CAN bus with FullCAN
     * Assumes you are using can1
     * @param [in] std_list_arr     Pointer to the array of the standard group id list
     *                              array should be of type uint16_t
     * @param [in] arraySize        Size of array. Use (sizeof(std_list_arr) / sizeof(*std_list_arr))
     * @returns true if the CAN bus is initialized
     * @code
     *     uint16_t accepted_msg_ids[] = {0x100, 0x102, 0x104, 0x200};
     *     iCAN_init_FULLCAN(accepted_msg_ids, sizeof(accepted_msg_ids) / sizeof(*accepted_msg_ids);
     * @endcode
     */



    /*
     * static const msg_hdr_t MASTER_TX_HEARTBEAT_HDR =              {  100, 1 };
    static const msg_hdr_t SENSOR_TX_INFO_SONARS_HDR =            {  600, 4 };
    static const msg_hdr_t MASTER_TX_MOTOR_CMD_HDR =              {  604, 2 };
    static const msg_hdr_t ANDROID_TX_STOP_GO_CMD_HDR =           {  700, 1 };
    static const msg_hdr_t ANDROID_TX_INFO_CHECKPOINTS_HDR =      {  708, 1 };
    static const msg_hdr_t ANDROID_TX_INFO_COORDINATES_HDR =      {  712, 8 };
    static const msg_hdr_t GPS_TX_INFO_HEADING_HDR =              {  716, 4 };
    static const msg_hdr_t GPS_TX_DESTINATION_REACHED_HDR =       {  720, 1 };
     // uint16_t accepted_msg_ids[] = {SENSOR_MASTER_REG, MASTER_MOTOR_COMMANDS};
    //iCAN_init_FULLCAN(accepted_msg_ids, sizeof(accepted_msg_ids) / sizeof(*accepted_msg_ids));
     */

//     uint32_t accepted_msg_ids[] = {(uint32_t) MASTER_TX_HEARTBEAT_HDR.mid,
//             (uint32_t) SENSOR_TX_INFO_SONARS_HDR.mid,
//             (uint32_t) MASTER_TX_MOTOR_CMD_HDR.mid,
//             (uint32_t) ANDROID_TX_STOP_GO_CMD_HDR.mid,
//             (uint32_t) ANDROID_TX_INFO_CHECKPOINTS_HDR.mid,
//             (uint32_t) ANDROID_TX_INFO_COORDINATES_HDR.mid,
//             (uint32_t) GPS_TX_INFO_HEADING_HDR.mid,
//             (uint32_t) GPS_TX_DESTINATION_REACHED_HDR.mid
//     };
    uint32_t accepted_msg_ids[] = {MASTER_TX_HEARTBEAT_HDR.mid,
                  SENSOR_TX_INFO_SONARS_HDR.mid,
                  MASTER_TX_MOTOR_CMD_HDR.mid,
                  ANDROID_TX_STOP_GO_CMD_HDR.mid,
                  ANDROID_TX_INFO_CHECKPOINTS_HDR.mid,
                  ANDROID_TX_INFO_COORDINATES_HDR.mid,
                 GPS_TX_INFO_HEADING_HDR.mid,
                  GPS_TX_DESTINATION_REACHED_HDR.mid
         };

       iCAN_init_FULLCAN(accepted_msg_ids, sizeof(accepted_msg_ids) / sizeof(*accepted_msg_ids));

    //initialize our switches, LE, and CAN_ST
        //initialize switches
        SW.init();

        //initialize LD display
        LD.init();

        //initialize acc sensor
        AS.init();

        //initialize our storage
       // CAN_ST.init();

    /**
     * A few basic tasks for this bare-bone system :
     *      1.  Terminal task provides gateway to interact with the board through UART terminal.
     *      2.  Remote task allows you to use remote control to interact with the board.
     *      3.  Wireless task responsible to receive, retry, and handle mesh network.
     *
     * Disable remote task if you are not using it.  Also, it needs SYS_CFG_ENABLE_TLM
     * such that it can save remote control codes to non-volatile memory.  IR remote
     * control codes can be learned by typing the "learn" terminal command.
     */
    scheduler_add_task(new terminalTask(PRIORITY_HIGH));

    /* Consumes very little CPU, but need highest priority to handle mesh network ACKs */
    scheduler_add_task(new wirelessTask(PRIORITY_CRITICAL));

    /*Add scheduler for our CAN task */
    //scheduler_add_task(new CAN_Handler_Tx(5));
   // scheduler_add_task(new CAN_Handler_Rx(5));

    //master control task

   // scheduler_add_task(new control_handler_task(5));

    //scheduler_add_task(new control_handler_task(5));


    /* Change "#if 0" to "#if 1" to run period tasks; @see period_callbacks.cpp */
#if 1
    scheduler_add_task(new periodicSchedulerTask());
#endif

    /* The task for the IR receiver */
    // scheduler_add_task(new remoteTask  (PRIORITY_LOW));

    /* Your tasks should probably used PRIORITY_MEDIUM or PRIORITY_LOW because you want the terminal
     * task to always be responsive so you can poke around in case something goes wrong.
     */

    /**
     * This is a the board demonstration task that can be used to test the board.
     * This also shows you how to send a wireless packets to other boards.
     */
#if 0
    scheduler_add_task(new example_io_demo());
#endif

    /**
     * Change "#if 0" to "#if 1" to enable examples.
     * Try these examples one at a time.
     */
#if 0
    scheduler_add_task(new example_task());
    scheduler_add_task(new example_alarm());
    scheduler_add_task(new example_logger_qset());
    scheduler_add_task(new example_nv_vars());
#endif

    /**
     * Try the rx / tx tasks together to see how they queue data to each other.
     */
#if 0
    scheduler_add_task(new queue_tx());
    scheduler_add_task(new queue_rx());
#endif

    /**
     * Another example of shared handles and producer/consumer using a queue.
     * In this example, producer will produce as fast as the consumer can consume.
     */
#if 0
    scheduler_add_task(new producer());
    scheduler_add_task(new consumer());
#endif

    /**
     * If you have RN-XV on your board, you can connect to Wifi using this task.
     * This does two things for us:
     *   1.  The task allows us to perform HTTP web requests (@see wifiTask)
     *   2.  Terminal task can accept commands from TCP/IP through Wifly module.
     *
     * To add terminal command channel, add this at terminal.cpp :: taskEntry() function:
     * @code
     *     // Assuming Wifly is on Uart3
     *     addCommandChannel(Uart3::getInstance(), false);
     * @endcode
     */
#if 0
    Uart3 &u3 = Uart3::getInstance();
    u3.init(WIFI_BAUD_RATE, WIFI_RXQ_SIZE, WIFI_TXQ_SIZE);
    scheduler_add_task(new wifiTask(Uart3::getInstance(), PRIORITY_LOW));
#endif
   // scheduler_add_task(new can_receive(PRIORITY_MEDIUM));
    scheduler_start(); ///< This shouldn't return
    return -1;
}
