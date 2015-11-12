/*
 * FullCan.cpp
 *
 *  Created on: Oct 23, 2015
 *      Author: Hassan
 */


#include "tasks.hpp"
#include "stdio.h"
#include "can.h"
#include "shared_handles.h"

#include <243_can/iCAN.hpp>
#include "L4_IO/can_definitions.hpp"


/*class can_receive : public scheduler_task
{
    public:
        can_receive(uint8_t priority) :
            scheduler_task("canBUS", 2048, priority)
        {
           //  Nothing to init
        }
        bool init(){
            can_t bus = can1;
            uint32_t baudrate_kbps = 100;
            uint16_t rxq_size = 50;
            uint16_t txq_size = 50;
            //const int ID1 = 0x123;

            if (CAN_init(bus, baudrate_kbps, rxq_size, txq_size,NULL,NULL)){
                if(CAN_fullcan_add_entry(bus, CAN_gen_sid(bus, 0x123), CAN_gen_sid(bus, 0x124))){
                    const can_std_id_t slist[]      = {
                                                        CAN_gen_sid(can1, 0x123), CAN_gen_sid(can1, 0x124),   // 2 entries
<<<<<<< HEAD
                                                        CAN_gen_sid(can1, 0x140), CAN_gen_sid(can1, 0x141)
=======
                                                        CAN_gen_sid(can1, 0x140), CAN_gen_sid(can1, 0x141),
                                                        CAN_gen_sid(can1, 0x702), //sensor values
                                                        CAN_gen_sid(can1, 0x704), //master command motor
>>>>>>> 44504a6ae0300853404a64324aa1c412d8e02a26
                                                      };
                    const can_std_grp_id_t sglist[] = { {CAN_gen_sid(can1, 0x01), CAN_gen_sid(can1, 0x200)}, // Group 1
                                                        {CAN_gen_sid(can2, 0x201), CAN_gen_sid(can2, 0x500)}  // Group 2
                                                      };
                    const can_ext_id_t *elist = NULL;
                    const can_ext_grp_id_t *eglist= NULL;
                    if(CAN_setup_filter(slist, 4, sglist, 2, elist, 0, eglist, 0)){
                        puts("\nIn Full Can Mode and Filter Setup!");
                    }

                }
                else{
                    puts("\nFULL CAN FAILED");
                }
                CAN_reset_bus(bus);
                puts("\nInitialization Successful\n");
                return true;
            }
            else{
                puts("\nFailed to Initialize");
                return false;
            }
        }

        bool run(void *p)
                {
                        can_t bus = can1;
                        //uint32_t timeout_ms = portMAX_DELAY;
                        can_fullcan_msg_t msg;
                       // can_fullcan_msg_t* msgPtr = CAN_fullcan_get_entry_ptr(CAN_gen_sid(bus, (uint16_t)SENSOR_MASTER_REG));
                        can_fullcan_msg_t* msgPtr = CAN_fullcan_get_entry_ptr(CAN_gen_sid(bus, (uint16_t)MASTER_COMMANDS_MOTOR));
                        can_msg_t raw_msg_tx = { 0 };

                        can_msg_t raw_msg_rx = { 0 };
//
//                        if(iCAN_rx(msg, (uint16_t) 0x702)) {
//                            printf("message received from SENSOR!!!\n");
//                        }


<<<<<<< HEAD
<<<<<<< HEAD
                        //can_msg_t msg_tx = { 0 };
=======
>>>>>>> fixed sensor receive issue
=======
>>>>>>> 44504a6ae0300853404a64324aa1c412d8e02a26

                         if(CAN_fullcan_read_msg_copy(msgPtr,&msg)){
                             puts("\nReceived!");
                             raw_msg_rx.msg_id = msg.msg_id;
                             raw_msg_rx.data = msg.data;
                             if(raw_msg_rx.msg_id == (uint32_t) 0x704)  {
                                 printf("HEEEELLLLOOOO!!!!!!!\n");
                             }
                             //pass the received message to the period_callback functions
<<<<<<< HEAD
<<<<<<< HEAD
                             //xQueueSend(scheduler_task::getSharedObject(shared_CAN_message_queue_receive), &msg, 0);
                             vTaskDelay(5000);
                             return true;
                             }
                        // else if(xQueueReceive(scheduler_task::getSharedObject(shared_CAN_message_queue_transmit), &msg_tx, 0)) {
                               //send data
                         //}
=======
=======
>>>>>>> 44504a6ae0300853404a64324aa1c412d8e02a26
                             xQueueSend(scheduler_task::getSharedObject(shared_CAN_message_queue_receive), &raw_msg_rx, 0);
                            // vTaskDelay(5000);
                             //return true;
                             }
<<<<<<< HEAD
>>>>>>> fixed sensor receive issue
=======
>>>>>>> 44504a6ae0300853404a64324aa1c412d8e02a26

                             else{
                                      puts ("\nNo message received!");
                                      //return false;
                                  }

                         if(xQueueReceive(scheduler_task::getSharedObject(shared_CAN_message_queue_transmit), &raw_msg_tx, 0)) {
                               //send data
                            // if(iCAN_tx(&raw_msg_tx, (uint32_t) MASTER_COMMANDS_MOTOR))  {
                            //     printf("message sent to motor!\n");
                            // }
                         }


<<<<<<< HEAD
<<<<<<< HEAD
=======

                       //  vTaskDelay(10);
>>>>>>> fixed sensor receive issue
=======

                       //  vTaskDelay(10);
>>>>>>> 44504a6ae0300853404a64324aa1c412d8e02a26
                }
};*/





///////////////////////////////////////////////////////////


class CAN_Handler_Rx : public scheduler_task {
    private:
        SemaphoreHandle_t mCAN_SemaphoreSignal_Rx =  xSemaphoreCreateBinary();
        can_msg_t mMessageReceive;

        uint32_t sourceId = 0x422;

    public:
        CAN_Handler_Rx(uint8_t priority) : scheduler_task("CAN_Handler_Rx", 2048, priority)   {

            addSharedObject(shared_CAN_Semaphore_Rx, mCAN_SemaphoreSignal_Rx);

        }

        bool init(void) {

            //initialize CAN interface
            CAN_init(can_t::can1, 100, 16, 16, NULL, NULL);

           CAN_bypass_filter_accept_all_msgs();

           //needs to be reset before we can use the CAN bus
            CAN_reset_bus(can_t::can1);

            return true;
        }

        bool run(void* p)   {
            mMessageReceive.msg_id = 0;

           // if(xSemaphoreTake(mCAN_SemaphoreSignal_Rx, 1))   {

                if(CAN_is_bus_off(can_t::can1)) {
                                  CAN_reset_bus(can_t::can1);
                                  printf("NOT CONNECTED IN THE CAN BUS! RESETTING...\n");
                             }
                else    {

                    CAN_rx(can_t::can1, &mMessageReceive, 1);
                   // printf("message_id: %" PRIu32 "\n", mMessageReceive.msg_id);
                    if((uint32_t) 0x704 == mMessageReceive.msg_id){
                        printf("HHHEEEEEEEELLLLOOOO!!!!!\n");
                    }
                    printf("received: %i\n", (uint8_t)mMessageReceive.data.qword);
                   // LE.setAll((uint8_t)mMessageReceive.data.qword);

                }



          //  }



            vTaskDelay(100);
            printf("Rx scheduler called.\n");

            return true;
       }


};


////////////////////////////////////////////////////////////////








