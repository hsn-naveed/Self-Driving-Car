//#include <stdio.h>
#include <stdint.h>
//#include "io.hpp"
#include "periodic_callback.h"
#include "file_logger.h"

#include"sensor.hpp"
#include "shared_handles.h"

/// This is the stack size used for each of the period tasks

const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);
QueueHandle_t canMessage =xQueueCreate(1,sizeof(can_msg_t));//added



void period_1Hz(void)
{
   // LE.toggle(1);

}

void period_10Hz(void)
{

    //SonarSensor.print();

    SonarSensor.Range_left();

    xSemaphoreTake(SonarSensor.middle_sem, portMAX_DELAY);
    SonarSensor.Range_middle();

    xSemaphoreTake(SonarSensor.right_sem, portMAX_DELAY);

      SonarSensor.Range_right();

      xSemaphoreTake(SonarSensor.rear_sem, portMAX_DELAY);
      SonarSensor.Range_rear();

      SonarSensor.CAN_send();

      delay_ms(100);

  //  LE.toggle(2);


#if hw4LightSensor
    averageOfLightSensor();
#endif
}

void period_100Hz(void)
{
    //LE.toggle(3);
}

void period_1000Hz(void)
{
   // LE.toggle(4);
}
