/*
 * sensor.c
 *
 *  Created Dec 11, 2015
 *      Author: Onyema
 **/

#include <sensor.hpp>
#include "can.h"
#include "can_message.h"

/// Variables used for DBC can communcation
// immediately initialize them, so that they don't create any crashing on the board
SENSOR_TX_INFO_SONARS_t *sensor_values_tx = new SENSOR_TX_INFO_SONARS_t{0};
can_msg_t *msg_tx = new can_msg_t {0};

int Left_trig_time, Middle_trig_time, Right_trig_time,
        Rear_trig_time;

int left_dist, middle_dist, right_dist,
        rear_dist;

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

    Range_rear();
    delay_ms(50);

    CAN_send();
    test();
}

void test(void){
   printf("\n\n\nLeft : %i \nMiddle : %i \nRight : %i \nRear : %i",
           left_dist, middle_dist, right_dist, rear_dist);
}

void CAN_send(void)
{


    /// Store the sensor values into the message that's to be sent
    sensor_values_tx->SENSOR_INFO_SONARS_left = (float) left_dist;
    sensor_values_tx->SENSOR_INFO_SONARS_middle = (float) middle_dist;
    sensor_values_tx->SENSOR_INFO_SONARS_right = (float) right_dist;
    sensor_values_tx->SENSOR_INFO_SONARS_rear = (float) rear_dist;

    /// The message that will be encoded with the stored values
    msg_hdr_t encoded_msg =  SENSOR_TX_INFO_SONARS_encode((uint64_t*)&(msg_tx->data.qword) , sensor_values_tx);

    /// Store the proper message ID
    encoded_msg.mid= SENSOR_TX_INFO_SONARS_HDR.mid;

    /// Send message with stored sensor values, and proper message id
    iCAN_tx(msg_tx, &encoded_msg);
    printf("\nCAN sent\n");
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
