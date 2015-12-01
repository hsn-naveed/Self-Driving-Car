/*
 * can_storage.cpp
 *
 *  Created on: Oct 26, 2015
 *      Author: Marvin
 */

#include <stdint.h>
#include "can_storage.hpp"


bool CAN_STORAGE::init()
{

    return true;
}


CAN_STORAGE::CAN_STORAGE()  {

    //Android
    mGoSignal = false;

    //Sensor
    sensor_data = new sen_msg_t;
    sensor_data->L = (uint8_t) 0;
    sensor_data->M = (uint8_t) 0;
    sensor_data->R = (uint8_t) 0;
    sensor_data->B = (uint8_t) 0;

    //Motor
    motor_data = new mast_mot_msg_t;

//    for (int i = 0; i < (int)SIZE_OF_SENSOR_ARRAY; i++){
//           mSensorValues[i] += i ;
//           //global_sensor_value[i] = mSensorValue[i];
//       }

    //GPS Coordinate
    gps_coords_curr = new gps_coordinate_msg_t;
    gps_coords_dest = new gps_coordinate_msg_t;

    //GPS Compass angle
    mAngleValue = new gps_heading_msg_t;
}

CAN_STORAGE::~CAN_STORAGE() {

    delete sensor_data;
    delete motor_data;
    delete gps_coords_curr;
    delete gps_coords_dest;

}


void CAN_STORAGE::setSafeSensorValues(){
    sensor_data->L = (uint8_t) 0x00;
  sensor_data->M = (uint8_t) 0x00;
  sensor_data->R = (uint8_t) 0x00;
  sensor_data->B = (uint8_t) 0x00;
}


