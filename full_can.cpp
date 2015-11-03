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

#include "iCAN.hpp"


class can_receive : public scheduler_task
{
    public:
        can_receive(uint8_t priority) :
            scheduler_task("canBUS", 2048, priority)
        {
            /* Nothing to init */
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
                                                        CAN_gen_sid(can1, 0x140), CAN_gen_sid(can1, 0x141),
                                                        CAN_gen_sid(can1, 0x702), //sensor values
                                                        CAN_gen_sid(can1, 0x704), //master command motor
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
                        can_fullcan_msg_t* msgPtr = CAN_fullcan_get_entry_ptr(CAN_gen_sid(bus, 0x123));

                        can_msg_t msg_tx = { 0 };

                        if(iCAN_rx(&msg, 0x702)) {
                            printf("message received from SENSOR!!!\n");
                        }

                         if(CAN_fullcan_read_msg_copy(msgPtr,&msg)){
                             puts("\nReceived!");
                             //pass the received message to the period_callback functions
                            // xQueueSend(scheduler_task::getSharedObject(shared_CAN_message_queue_receive), &msg, 0);
                            // vTaskDelay(5000);
                             return true;
                             }
                         //else if(xQueueReceive(scheduler_task::getSharedObject(shared_CAN_message_queue_transmit), &msg_tx, 0)) {
                               //send data
                       //  }

                         else{
                             puts ("\nNo message received!");
                             return false;
                         }

                       //  vTaskDelay(10);
                }
};



