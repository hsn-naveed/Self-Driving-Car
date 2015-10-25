
#include <stdio.h>
#include "utilities.h"
#include "tasks.hpp"
#include "examples/examples.hpp"
#include "adc0.h"
#include "io.hpp"
#include <math.h>
#include "uart2.hpp"

char p;
float adc_middle ;
const float to_inches = 6.4; // devide adc reading by 6.4 (when 3.3V Vcc used) to get the distance in inches
double sonar_middle_inches;

int main(void)
{

//int   sensor (left_sensor)

LPC_PINCON->PINSEL3 |= (3<<28); //ADC 3

/// Analog_Sonar
  while(1){

        adc_middle = adc0_get_reading(4);

        sonar_middle_inches = floor(adc_middle/to_inches); //converts the sensor value to inches, pretty exact!

        printf("\n adc value is : %.11f",sonar_middle_inches);
        delay_ms(250);
    }



/*
/// Analog_IR
//////////////////////
    LPC_PINCON->PINSEL3 |= (3<<28); //ADC 3

      while(1)
      {
           for(int i =0; i<10; i++)
           {
               adc_middle = adc0_get_reading(4);
               sonar_middle_inches = floor(adc_middle/to_inches); //converts the sensor value to inches, pretty exact!
               sonar_middle_inches = + sonar_middle_inches;
               //adc_middle = +adc_middle;

               delay_ms(60);
           }
           sonar_middle_inches =  sonar_middle_inches/10;

           printf("\n adc value is : %.11f", sonar_middle_inches);
           delay_ms(1200);
       }
   //////////////////////
*/


    scheduler_add_task(new terminalTask(PRIORITY_HIGH));

    /* Consumes very little CPU, but need highest priority to handle mesh network ACKs */
    scheduler_add_task(new wirelessTask(PRIORITY_CRITICAL));

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

    scheduler_start(); ///< This shouldn't return
    return -1;
}
