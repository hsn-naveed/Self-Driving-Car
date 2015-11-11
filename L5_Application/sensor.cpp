/*
 * sensor.c
 *
 *  Created on: Nov 6, 2015
 *      Author: Arlen
 */

#include <sensor.hpp>



int Left_trig_time, Middle_trig_time, Right_trig_time,
        Rear_trig_time;

int left_dist, middle_dist, right_dist,
        rear_dist;

//int distL[2];

//int i =0;

GPIO  Left_en(P2_7);   // left RX pin
GPIO  Middle_en(P2_6); // Middle
GPIO  Right_en(P0_30); // Right
GPIO  Rear_en(P0_29);  //Rear


can_msg_t *sonarMsg = new can_msg_t ;

void Range(){

    Range_right();
    delay_ms(50);

    Range_middle();
    delay_ms(50);

    Range_left();
    delay_ms(50);

/*
    Range_rear();
    delay_ms(50);
*/
    CAN_send();
    test();
}

void test(void){

   printf("\n\n\nLeft : %i \nMiddle : %i \nRight : %i \nRear : %i",
           left_dist, middle_dist, right_dist, rear_dist);
}


/*
void store(int in)
{
    if(i<2)
    {
        distL[i] = in;
        i++;
    }
    i = 0;//reset
}
*/

/*
void filter(int in, int out)
{


    if()

}


*/

void CAN_send(void)
{
    sen_msg_t* sensor_values = (sen_msg_t*) &sonarMsg->data.bytes[0];

    sonarMsg->msg_id = (uint32_t) 0x702;

    sensor_values->L = (uint8_t) left_dist;
    sensor_values->M = (uint8_t) middle_dist;
    sensor_values->R = (uint8_t) right_dist;
    sensor_values->B = (uint8_t) rear_dist;

    iCAN_tx(sonarMsg, (uint16_t)0x702);
    printf("\n\nCAN sent");

    //iCAN_tx(&sonar,uint32_t (0x702)); //transmit over the can
    //xQueueSend(scheduler_task::getSharedObject(shared_CANsend), &sonarMsg, 0);
}

void calc_dist_left(void)
{
    left_dist = ((sys_get_uptime_us() - Left_trig_time)/147) ; //each 147uS is 1 inch (Datasheet)
//Main problem was using this type of timer and puting intrrrupt line in while loop!USE sys_get_uptime_us
//left_dist = (lpc_timer_get_value(lpc_timer0)/147)-2; //each 147uS is 1 inch (Datasheet)
 //store (left_dist);
  //  xSemaphoreGiveFromISR(Range_en , NULL);
}
void calc_dist_middle(void)
{
    middle_dist = ((sys_get_uptime_us() - Middle_trig_time) / 147) ; //each 147uS is 1 inch (Datasheet)
  //  xSemaphoreGiveFromISR(Range_en , NULL);
}
void calc_dist_right(void)
{
    right_dist = ((sys_get_uptime_us() - Right_trig_time) / 147) ; //each 147uS is 1 inch (Datasheet)
   // xSemaphoreGiveFromISR(Range_en , NULL);
}
void calc_dist_rear(void)
{
    rear_dist = ((sys_get_uptime_us() - Rear_trig_time) / 147) ; //each 147uS is 1 inch (Datasheet)

  //  xSemaphoreGiveFromISR(Range_en , NULL);
}
void Range_left(void)
{
  //  if(xSemaphoreTake(Range_en,38 )) {
 //   delay_ms(15);
    Left_en.setHigh(); // enable Ranging   (enable left sonar)
    delay_us(21); //hold high  >20uS to enable ranging
    Left_trig_time = sys_get_uptime_us(); //get timer at the moment ranging starts
    Left_en.setLow(); // disable ranging of left sonar

 //   }
}
void Range_middle(void)
{
   // if(xSemaphoreTake(Range_en, 38)) {
    //delay_ms(15);
    Middle_en.setHigh(); // enable Ranging   (enable left sonar)
    delay_us(21); //hold high  >20uS to enable ranging
    Middle_trig_time = sys_get_uptime_us(); //get timer at the moment ranging starts
    Middle_en.setLow(); // disable ranging of left sonar
   // }
}
void Range_right(void)
{
 //   if(xSemaphoreTake(Range_en, 38)) {
   // delay_ms(15);
    Right_en.setHigh(); // enable Ranging   (enable left sonar)
    delay_us(21); //hold high  >20uS to enable ranging
    Right_trig_time = sys_get_uptime_us(); //get timer at the moment ranging starts
    Right_en.setLow(); // disable ranging of left sonar
   // }
    }
void Range_rear(void)
{
    //delay_ms(15);
    Rear_en.setHigh(); // enable Ranging   (enable left sonar)
    delay_us(21); //hold high  >20uS to enable ranging
    Rear_trig_time = sys_get_uptime_us(); //get timer at the moment ranging starts
    Rear_en.setLow(); // disable ranging of left sonar
}


