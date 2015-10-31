//2 sonar one not working rihgt
#include "tasks.hpp"
#include "examples/examples.hpp"
#include "eint.h"
#include <stdio.h>
#include"gpio.hpp"
#include"utilities.h" //delay lib
#include"io.hpp"
#include"lpc_timers.h"
#include "lpc_sys.h"


int Left_trig_time,Middle_trig_time,Right_trig_time,Rear_trig_time,left_dist,middle_dist,right_dist,rear_dist;

const uint8_t p2_0 = 0; // will be pulled Low when object detected
const uint8_t p2_1 = 1; // will be pulled Low when object detected

GPIO Left_en(P0_0);   // left Sonar RX enable pin
GPIO Middle_en(P0_1); // Middle Sonar RX enable pin

SemaphoreHandle_t left_sem= xSemaphoreCreateMutex(); //**can't use the same semaphor for more than 2 tasks!
SemaphoreHandle_t middle_sem= xSemaphoreCreateMutex(); //another semaphore for the




void calc_dist_left(void)
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

     xSemaphoreGive(left_sem);
}

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

int main(void)
{
    eint3_enable_port2(p2_0, eint_falling_edge, calc_dist_left); //wait for the interrupt
    eint3_enable_port2(p2_1, eint_falling_edge, calc_dist_middle); //wait for the interrupt


    Left_en.setAsOutput(); // set p0.0 as an output pin to enable or disable Left Sonar
    Middle_en.setAsOutput();

    delay_ms(251); // 250ms after power up RX is ready to receive commands!


    while(1)
    {
        // trigger only once until falling edge arrives
        Range_left();

        xSemaphoreTake(middle_sem, portMAX_DELAY);
        Range_middle();

        xSemaphoreTake(left_sem, portMAX_DELAY);
        delay_ms(2000);
    }

    // p01.setAsOutput(); //sets p0.1 as output

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
