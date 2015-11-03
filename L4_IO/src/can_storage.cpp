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
    sensor_data = new sen_msg_t;
    sensor_data->L = (uint8_t) MINIMUM_SENSOR_VALUE;
    sensor_data->M = (uint8_t) MINIMUM_SENSOR_VALUE;
    sensor_data->R = (uint8_t) MINIMUM_SENSOR_VALUE;
    sensor_data->B = (uint8_t) MINIMUM_SENSOR_VALUE;

    //Motor
    motor_data = new mast_mot_msg_t;

//    for (int i = 0; i < (int)SIZE_OF_SENSOR_ARRAY; i++){
//           mSensorValues[i] += i ;
//           //global_sensor_value[i] = mSensorValue[i];
//       }
}

CAN_STORAGE::~CAN_STORAGE() {

    delete sensor_data;
    delete motor_data;
}

//void CAN_STORAGE::setSensorValues(uint8_t arr[], int size)  {
//    if(size > (int) SIZE_OF_SENSOR_ARRAY) { size = (int) SIZE_OF_SENSOR_ARRAY ; }
//    for (int i = 0; i < size; i++){
//               mSensorValues[i] = 0;
//               //global_sensor_value[i] = mSensorValue[i];
//           }
//}
