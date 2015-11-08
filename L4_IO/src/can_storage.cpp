/*
 * can_storage.cpp
 *
 *  Created on: Oct 26, 2015
 *      Author: Marvin
 */

#include <can_definitions.hpp>
#include <stdint.h>
#include "can_storage.hpp"


bool CAN_STORAGE::init()
{

    return true;
}


CAN_STORAGE::CAN_STORAGE()  {

    //Android
    mGoSignal = 0;

    //Motor
    mMotorSpeed = 0;

    //Sensor
    for (int i = 0; i < (int)SIZE_OF_SENSOR_ARRAY; i++){
           mSensorValues[i] += i ;
           //global_sensor_value[i] = mSensorValue[i];
       }
}

void CAN_STORAGE::setSensorValues(uint8_t arr[], int size)  {
    if(size > (int) SIZE_OF_SENSOR_ARRAY) { size = (int) SIZE_OF_SENSOR_ARRAY ; }
    for (int i = 0; i < size; i++){
               mSensorValues[i] = 0;
               //global_sensor_value[i] = mSensorValue[i];
           }
}


void CAN_STORAGE::setSafeSensorValues(){
    sensor_data->L = (uint8_t) 0xff;
  sensor_data->M = (uint8_t) 0xff;
  sensor_data->R = (uint8_t) 0xff;
  sensor_data->B = (uint8_t) 0xff;
}

//void CAN_STORAGE::setSensorValues(uint8_t arr[], int size)  {
//    if(size > (int) SIZE_OF_SENSOR_ARRAY) { size = (int) SIZE_OF_SENSOR_ARRAY ; }
//    for (int i = 0; i < size; i++){
//               mSensorValues[i] = 0;
//               //global_sensor_value[i] = mSensorValue[i];
//           }
//}

