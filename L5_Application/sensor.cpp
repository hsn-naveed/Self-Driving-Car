/*
 * sensor.c
 *
 *  Created on: Nov 6, 2015
 *      Author: Arlen
 */

#include <sensor.hpp>



int Left_trig_time, Middle_trig_time, Right_trig_time,
    Middle_int_time,
        Rear_trig_time;

int left_dist, middle_dist, right_dist,
        rear_dist;

GPIO  Left_en(P2_7);   // left RX pin
GPIO  Middle_en(P2_6); // Middle
GPIO  Right_en(P0_30); // Right
GPIO  Rear_en(P0_29);  //Rear


SemaphoreHandle_t Range_en = xSemaphoreCreateBinary(); // Create the semaphore

//  xSemaphoreTake(Range_en,0);



/*
SemaphoreHandle_t left_sem   = xSemaphoreCreateMutex();   //**can't use the same semaphor for more than 2 tasks!
SemaphoreHandle_t middle_sem = xSemaphoreCreateMutex();   //give to middle
SemaphoreHandle_t right_sem  = xSemaphoreCreateMutex();   //give to right
SemaphoreHandle_t rear_sem   = xSemaphoreCreateMutex();   //give it to rear
*/

void calc_dist_left(void)
{
    Middle_int_time = sys_get_uptime_us();
    //printf("\n >System time in calc dist is: %i", Middle_int_time);

    left_dist = ((Middle_int_time - Left_trig_time)/147) - 2; //each 147uS is 1 inch (Datasheet)
//Main problem was using this type of timer and puting intrrrupt line in while loop!USE sys_get_uptime_us
//left_dist = (lpc_timer_get_value(lpc_timer0)/147)-2; //each 147uS is 1 inch (Datasheet)


    printf("\n \n Left ninterrupt occured \n Left dist in inches is : %i", left_dist);


    xSemaphoreGiveFromISR(Range_en , NULL);
}

void calc_dist_middle(void)
{

    middle_dist = ((sys_get_uptime_us() - Middle_trig_time) / 147) - 2; //each 147uS is 1 inch (Datasheet)

    printf("\n \n Middle ninterrupt occured");
    printf("\n Middle distance in inches is : %i", middle_dist);

    //xSemaphoreGive(SonarSensor.right_sem);
}
void calc_dist_right(void)
{
    right_dist = ((sys_get_uptime_us() - Right_trig_time) / 147) - 2; //each 147uS is 1 inch (Datasheet)

    printf("\n \n Right ninterrupt occured");
    printf("\n Right distance in inches is : %i", right_dist);

   // xSemaphoreGive(SonarSensor.rear_sem);
}

void calc_dist_rear(void)
{
    rear_dist = ((sys_get_uptime_us() - Rear_trig_time) / 147) - 2; //each 147uS is 1 inch (Datasheet)

    printf("\n \n Rear ninterrupt occured");
    printf("\n Rear distance in inches is : %i", rear_dist);

    //xSemaphoreGive(SonarSensor.left_sem);
}

void Range_left(void)
{

    if(xSemaphoreTake(Range_en, 9999999)) {

 //   delay_ms(50);
    Left_en.setHigh(); // enable Ranging   (enable left sonar)
    delay_us(20); //hold high  >20uS to enable ranging
    Left_trig_time = sys_get_uptime_us(); //get timer at the moment ranging starts
   // printf ("\n Left trig time is : %i", Left_trig_time);
    Left_en.setLow(); // disable ranging of left sonar

    }
}

/*
void Range_middle(void)
{
    delay_ms(50);
    Middle_en.setHigh(); // enable Ranging   (enable left sonar)
    delay_us(21); //hold high  >20uS to enable ranging
    Middle_trig_time = sys_get_uptime_us(); //get timer at the moment ranging starts

    Middle_en.setLow(); // disable ranging of left sonar
}

void Range_right(void)
{
    delay_ms(50);
    Right_en.setHigh(); // enable Ranging   (enable left sonar)
    delay_us(21); //hold high  >20uS to enable ranging
    Right_trig_time = sys_get_uptime_us(); //get timer at the moment ranging starts

    Right_en.setLow(); // disable ranging of left sonar
}

void Range_rear(void)
{
    delay_ms(50);
    Rear_en.setHigh(); // enable Ranging   (enable left sonar)
    delay_us(21); //hold high  >20uS to enable ranging
    Rear_trig_time = sys_get_uptime_us(); //get timer at the moment ranging starts

    Rear_en.setLow(); // disable ranging of left sonar
}
*/


