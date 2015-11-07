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

    scheduler_add_task(new can_receive(PRIORITY_MEDIUM));

    scheduler_start(); ///< This shouldn't return

    return -1;
}
