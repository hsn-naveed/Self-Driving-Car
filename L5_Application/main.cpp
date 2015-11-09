<<<<<<< HEAD
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
#include <full_can.cpp>
#include <243_can/iCAN.hpp>
#include "tasks.hpp"
#include "examples/examples.hpp"
#include "io.hpp"
#include "stdio.h"
#include "file_logger.h"
#include "can.h"
#include <inttypes.h>

//This is a test - MARVIN

#include "master_control.hpp"




//class CAN_Handler_Tx : public scheduler_task {
//    private:
//
//        QueueHandle_t mCAN_QueueHandler = xQueueCreate(1, sizeof(int));
//
//    public:
//        CAN_Handler_Tx(uint8_t priority) : scheduler_task("CAN_Handler_Tx", 2048, priority)   {
//
//        }
//
//        bool init(void) {
//
//
//            addSharedObject(shared_LEDSignalForCAN, mCAN_QueueHandler);
//            mCAN_QueueHandler = getSharedObject(shared_LEDSignalForCAN);
//
//           // SW.init();
//
//
//            return true;
//        }
//
//        bool run(void* p)   {
//
//           // char msg_sw;
//            int msg_sw;
//            can_msg_t msg;
//            msg.msg_id = 0x414;
//            msg.frame_fields.is_29bit = 1;
//            msg.frame_fields.data_len = 8;       // Send 8 bytes
//
//            if (xQueueReceive(mCAN_QueueHandler, &msg_sw, 1))  {
//                msg.data.qword =  msg_sw;
//                CAN_tx(can_t::can1, &msg, 1);
//               // printf("CAN message sent: %i!!!\n", msg_sw);
//            }
//            //printf("exited tx\n");
//
//           return true;
//        }
//
//};
//
//
//class CAN_Handler_Rx : public scheduler_task {
//    private:
//        SemaphoreHandle_t mCAN_SemaphoreSignal_Rx =  xSemaphoreCreateBinary();
//        can_msg_t mMessageReceive;
//
//        uint32_t sourceId = 0x422;
//
//    public:
//        CAN_Handler_Rx(uint8_t priority) : scheduler_task("CAN_Handler_Rx", 2048, priority)   {
//
//            addSharedObject(shared_CAN_Semaphore_Rx, mCAN_SemaphoreSignal_Rx);
//
//        }
//
//        bool init(void) {
//
//            //initialize CAN interface
//            CAN_init(can_t::can1, 100, 16, 16, NULL, NULL);
//
//           CAN_bypass_filter_accept_all_msgs();
//
//           //needs to be reset before we can use the CAN bus
//            CAN_reset_bus(can_t::can1);
//
//            return true;
//        }
//
//        bool run(void* p)   {
//
//
//            if(xSemaphoreTake(mCAN_SemaphoreSignal_Rx, 1))   {
//
//                if(CAN_is_bus_off(can_t::can1)) {
//                                  CAN_reset_bus(can_t::can1);
//                                  printf("NOT CONNECTED IN THE CAN BUS! RESETTING...\n");
//                             }
//                else    {
//
//                    CAN_rx(can_t::can1, &mMessageReceive, 1);
//                    printf("message_id: %" PRIu32 "\n", mMessageReceive.msg_id);
//                    if((uint32_t) 0x704 == mMessageReceive.msg_id){
//                        printf("HHHEEEEEEEELLLLOOOO!!!!!\n");
//                    }
//                    printf("received: %i\n", (uint8_t)mMessageReceive.data.qword);
//                    LE.setAll((uint8_t)mMessageReceive.data.qword);
//
//                }
//
//
//
//            }
//            vTaskDelay(100);
//            printf("Rx scheduler called.\n");
//
//            return true;
//        }
//
//
//};










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

    uint16_t accepted_msg_ids[] = {SENSOR_MASTER_REG, MASTER_COMMANDS_MOTOR};
    iCAN_init_FULLCAN(accepted_msg_ids, sizeof(accepted_msg_ids) / sizeof(*accepted_msg_ids));

    //initialize our switches, LE, and CAN_ST
    //initialize switches
        SW.init();

        //initialize LD display
        LD.init();

        CAN_ST.init();
=======
#include "tasks.hpp"
#include "eint.h"


//#include "243_can/CAN_structs.h"
#include "FullCan.cpp"
//#include"iCAN.hpp"
#include"sensor.hpp"

//int Left_trig_time,Middle_trig_time,Right_trig_time,Rear_trig_time;

/*
int Left_trig_time,Middle_trig_time,Right_trig_time,Rear_trig_time,
    left_dist,
    middle_dist,
    right_dist,
    rear_dist;
*/

/*
can_msg_t sonar = {0};

//interrupt enabled GPIO ports Falling Edge
const uint8_t p2_0  = 0; // Left
const uint8_t p2_1  = 1; // Middle
const uint8_t p2_2  = 2; //Right
const uint8_t p2_3  = 3; // Rear

//Sonar enable pins init , pull up for >20uS to start ranging
GPIO Left_en(P2_7);   // left
GPIO Middle_en(P2_6); // Middle
GPIO Right_en(P0_30); // Right
GPIO Rear_en(P0_29);  // Rear
*/

//------------------------------
/*
SemaphoreHandle_t left_sem   = xSemaphoreCreateMutex();  //**can't use the same semaphor for more than 2 tasks!
SemaphoreHandle_t middle_sem = xSemaphoreCreateMutex();//give to middle
SemaphoreHandle_t right_sem = xSemaphoreCreateMutex(); //give to right
SemaphoreHandle_t rear_sem = xSemaphoreCreateMutex();      //give it to rear
*/
//------------------------------------

/*void calc_dist_left(void)
{
   left_dist = ((sys_get_uptime_us() - Left_trig_time)/147)-2; //each 147uS is 1 inch (Datasheet)
//Main problem was using this type of timer and puting intrrrupt line in while loop!USE sys_get_uptime_us
//left_dist = (lpc_timer_get_value(lpc_timer0)/147)-2; //each 147uS is 1 inch (Datasheet)

    printf("\n \n Left ninterrupt occured");
    printf("\n Left dist in inches is : %i", left_dist);

     xSemaphoreGive(middle_sem);
}

void calc_dist_middle(void)
{
    middle_dist = ((sys_get_uptime_us() - Middle_trig_time)/147)-2; //each 147uS is 1 inch (Datasheet)

    printf("\n \n Middle ninterrupt occured");
    printf("\n Middle distance in inches is : %i", middle_dist);

     xSemaphoreGive(right_sem);
}

void calc_dist_right(void)
{
    right_dist = ((sys_get_uptime_us() - Right_trig_time)/147)-2; //each 147uS is 1 inch (Datasheet)

    printf("\n \n Right ninterrupt occured");
    printf("\n Right distance in inches is : %i", right_dist);

     xSemaphoreGive(rear_sem);
}

void calc_dist_rear(void)
{
    rear_dist = ((sys_get_uptime_us() - Rear_trig_time)/147)-2; //each 147uS is 1 inch (Datasheet)

    printf("\n \n Rear ninterrupt occured");
    printf("\n Rear distance in inches is : %i", rear_dist);

     xSemaphoreGive(left_sem);
}*/


//-----------------------------------------------------------------
/*
void Range_left(void)
{
    delay_ms(50);
    Left_en.setHigh(); // enable Ranging   (enable left sonar)
    delay_us(21);  //hold high  >20uS to enable ranging
    Left_trig_time = sys_get_uptime_us();  //get timer at the moment ranging starts

    Left_en.setLow();   // disable ranging of left sonar
}

void Range_middle(void)
{
    delay_ms(50);
    Middle_en.setHigh(); // enable Ranging   (enable left sonar)
    delay_us(21);  //hold high  >20uS to enable ranging
    Middle_trig_time = sys_get_uptime_us();  //get timer at the moment ranging starts

    Middle_en.setLow();   // disable ranging of left sonar
}

void Range_right(void)
{
    delay_ms(50);
    Right_en.setHigh(); // enable Ranging   (enable left sonar)
    delay_us(21);  //hold high  >20uS to enable ranging
    Right_trig_time = sys_get_uptime_us();  //get timer at the moment ranging starts

    Right_en.setLow();   // disable ranging of left sonar
}

void Range_rear(void)
{
    delay_ms(50);
    Rear_en.setHigh(); // enable Ranging   (enable left sonar)
    delay_us(21);  //hold high  >20uS to enable ranging
    Rear_trig_time = sys_get_uptime_us();  //get timer at the moment ranging starts

    Rear_en.setLow();   // disable ranging of left sonar
}

void CAN_send(void)
{
    sen_msg_t* sensor_values = (sen_msg_t*) & sonar.data.bytes[0];

     sonar.msg_id= uint32_t(0x702);
     sensor_values->L = (uint8_t) left_dist;
     sensor_values->M = (uint8_t) middle_dist;
     sensor_values->R = (uint8_t) right_dist;
     sensor_values->B = (uint8_t) rear_dist;

   //  iCAN_tx(&sonar,uint32_t (0x702)); //transmit over the can
  //   xQueueSend(scheduler_task::getSharedObject(shared_CANsend), &sonar,0);
}


*/
//-----------------------------------------------------
//sensor SonarSensor = sensor::getInstance();
int main(void)
{

  //  SonarSensor.init();

//Interrupt init

    eint3_enable_port2(0,eint_falling_edge, SonarSensor.calc_dist_left); //Left Sonar
    eint3_enable_port2(1, eint_falling_edge, SonarSensor.calc_dist_middle); //Middle Sonar
    eint3_enable_port2(2, eint_falling_edge, SonarSensor.calc_dist_right); //Right Sonar
    eint3_enable_port2(3, eint_falling_edge, SonarSensor.calc_dist_rear); //Rear Sonar*/



    sensor::Left_en.setAsOutput(); // set p0.0 as an output pin to enable or disable Left Sonar
    sensor::Middle_en.setAsOutput();
    sensor::Right_en.setAsOutput();
    sensor::Rear_en.setAsOutput();

    delay_ms(251); // 250ms after power up RX is ready to receive commands!

#if 1
    scheduler_add_task(new periodicSchedulerTask());
#endif

>>>>>>> 89e575a84a94b67be5599bb2db3c8e9a88529312

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

<<<<<<< HEAD
    /*Add scheduler for our CAN task */
    //scheduler_add_task(new CAN_Handler_Tx(5));
   // scheduler_add_task(new CAN_Handler_Rx(5));

    //master control task
    //scheduler_add_task(new control_handler_task(5));


    /* Change "#if 0" to "#if 1" to run period tasks; @see period_callbacks.cpp */
#if 1
    scheduler_add_task(new periodicSchedulerTask());
#endif

=======
>>>>>>> 89e575a84a94b67be5599bb2db3c8e9a88529312
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
<<<<<<< HEAD
   // scheduler_add_task(new can_receive(PRIORITY_MEDIUM));
=======

    scheduler_add_task(new can_receive(PRIORITY_MEDIUM));

>>>>>>> 89e575a84a94b67be5599bb2db3c8e9a88529312
    scheduler_start(); ///< This shouldn't return

    return -1;
}
