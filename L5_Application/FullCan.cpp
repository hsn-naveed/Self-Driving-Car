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
#include "full_can_api.h"
#include "io.hpp"
//#include "eint.h"

class can_receive : public scheduler_task
{
    private :

        QueueHandle_t canMessage =xQueueCreate(1,sizeof(can_msg_t));//added

    public:
        can_receive(uint8_t priority) :
        scheduler_task("canBUS", 2048, priority)
        {
            /* Nothing to init */
        }
        bool init()
        {
            addSharedObject(shared_CANsend,canMessage);//added
            canMessage = getSharedObject(shared_CANsend);//added
            full_can_api::bus_init();

        }

        bool run(void *p)
                {
                can_msg_t Sonar_Send;

                xQueueReceive(canMessage, &Sonar_Send,0);
              full_can_api::can_send(&Sonar_Send, 0);


              if( full_can_api::can_send(&Sonar_Send, 0))
                {
                  printf("dataSent to CAN successfully");
                      return true;
                  }
              }



                  /*      can_msg_t SonarReceive;
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
                         }

                         if(xQueueReceive(scheduler_task::getSharedObject(shared_CANsend),&SonarReceive,0))
                           {
                             if(iCAN_tx(&SonarReceive, (uint32_t)0x702))
                             {
                                 printf("dataSent to CAN successfully");
                                 return true;
                             }
                         }
*/
                       //  vTaskDelay(10);

};



