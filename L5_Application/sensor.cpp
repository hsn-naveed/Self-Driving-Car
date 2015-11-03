
#include "sensor.hpp"
#include "tasks.hpp"
#include "shared_handles.h"
#include "can.h"




SemaphoreHandle_t left_sem   = xSemaphoreCreateMutex();  //**can't use the same semaphor for more than 2 tasks!
SemaphoreHandle_t middle_sem = xSemaphoreCreateMutex();//give to middle
SemaphoreHandle_t right_sem = xSemaphoreCreateMutex(); //give to right
SemaphoreHandle_t rear_sem = xSemaphoreCreateMutex();      //give it to rear


void sensor::calc_dist_left(void)
{
   left_dist = ((sys_get_uptime_us() - Left_trig_time)/147)-2; //each 147uS is 1 inch (Datasheet)
//Main problem was using this type of timer and puting intrrrupt line in while loop!USE sys_get_uptime_us
//left_dist = (lpc_timer_get_value(lpc_timer0)/147)-2; //each 147uS is 1 inch (Datasheet)

    printf("\n \n Left ninterrupt occured");
    printf("\n Left dist in inches is : %i", left_dist);

     xSemaphoreGive(middle_sem);
}

 void sensor::calc_dist_middle(void)
{
    middle_dist = ((sys_get_uptime_us() - Middle_trig_time)/147)-2; //each 147uS is 1 inch (Datasheet)

    printf("\n \n Middle ninterrupt occured");
    printf("\n Middle distance in inches is : %i", middle_dist);

     xSemaphoreGive(right_sem);
}
 void sensor::calc_dist_right(void)
{
    right_dist = ((sys_get_uptime_us() - Right_trig_time)/147)-2; //each 147uS is 1 inch (Datasheet)

    printf("\n \n Right ninterrupt occured");
    printf("\n Right distance in inches is : %i", right_dist);

     xSemaphoreGive(rear_sem);
}

 void sensor::calc_dist_rear(void)
{
    rear_dist = ((sys_get_uptime_us() - Rear_trig_time)/147)-2; //each 147uS is 1 inch (Datasheet)

    printf("\n \n Rear ninterrupt occured");
    printf("\n Rear distance in inches is : %i", rear_dist);

     xSemaphoreGive(left_sem);
}

void sensor::Range_left(void)
{
    delay_ms(50);
    Left_en.setHigh(); // enable Ranging   (enable left sonar)
    delay_us(21);  //hold high  >20uS to enable ranging
    Left_trig_time = sys_get_uptime_us();  //get timer at the moment ranging starts

    Left_en.setLow();   // disable ranging of left sonar
}

void sensor::Range_middle(void)
{
    delay_ms(50);
    Middle_en.setHigh(); // enable Ranging   (enable left sonar)
    delay_us(21);  //hold high  >20uS to enable ranging
    Middle_trig_time = sys_get_uptime_us();  //get timer at the moment ranging starts

    Middle_en.setLow();   // disable ranging of left sonar
}

void sensor::Range_right(void)
{
    delay_ms(50);
    Right_en.setHigh(); // enable Ranging   (enable left sonar)
    delay_us(21);  //hold high  >20uS to enable ranging
    Right_trig_time = sys_get_uptime_us();  //get timer at the moment ranging starts

    Right_en.setLow();   // disable ranging of left sonar
}

void sensor::Range_rear(void)
{
    delay_ms(50);
    Rear_en.setHigh(); // enable Ranging   (enable left sonar)
    delay_us(21);  //hold high  >20uS to enable ranging
    Rear_trig_time = sys_get_uptime_us();  //get timer at the moment ranging starts

    Rear_en.setLow();   // disable ranging of left sonar
}

void sensor::CAN_send(void)
{
    sen_msg_t* sensor_values = (sen_msg_t*) & sonarMsg->data.bytes[0];

     sonarMsg->msg_id= (uint32_t)0x702;
     sensor_values->L = (uint8_t) left_dist;
     sensor_values->M = (uint8_t) middle_dist;
     sensor_values->R = (uint8_t) right_dist;
     sensor_values->B = (uint8_t) rear_dist;

     //iCAN_tx(&sonar,uint32_t (0x702)); //transmit over the can
     xQueueSend(scheduler_task::getSharedObject(shared_CANsend), &sonarMsg,0);
}

//sensor::sensor()
//{
// left_dist=0;
//
//}

bool sensor::init(void){
    return true;
}
/*
void sensor::print(void){

    printf("testTEST");
}
*/
