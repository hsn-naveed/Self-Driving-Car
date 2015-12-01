/*
 * can_storage.hpp
 *
 *  Created on: Oct 26, 2015
 *      Author: Marvin
 */

#ifndef L4_IO_CAN_STORAGE_HPP_
#define L4_IO_CAN_STORAGE_HPP_

#include <stdint.h>
#include <L3_Utils/singleton_template.hpp>

//#include "L5_Application/can_message.c"
#include "243_can/CAN_structs.h"
/**
 * LED class used to control the Board's 8 output LEDs
 *
 * @ingroup BoardIO
 */

class CAN_STORAGE : public SingletonTemplate <CAN_STORAGE>
{
    public:
        bool init(); ///< Initializes this device, @returns true if successful


        //Sensor
        void setSafeSensorValues();

        //Android
        void setGoSignal(bool signal) {mGoSignal = signal;}
        bool getGoSignal() {return mGoSignal;}


        //Sensor
        sen_msg_t* sensor_data;

        //Motor
        //mast_mot_msg_t* motor_data;
        //MASTER_TX_MOTOR_CMD_t* motor_data;

        //current car coordinates sent by gps
        gps_coordinate_msg_t* gps_coords_curr;

        //destination coordinates sent by android
        gps_coordinate_msg_t* gps_coords_dest;

        //car bearing as measured by the compass
        gps_heading_msg_t* mAngleValue;

    private:

        bool mGoSignal;


        ///< Private constructor of this Singleton class
          /*  CAN_STORAGE() :  mMotorSpeed(0)  {

                for (int i = 0; i < (int)SIZE_OF_SENSOR_ARRAY; i++){
                       mSensorValue[i] = 0;
                       //global_sensor_value[i] = mSensorValue[i];
                   }

            }*/
        CAN_STORAGE();
        ~CAN_STORAGE();

        friend class SingletonTemplate<CAN_STORAGE>;  ///< Friend class used for Singleton Template
};

#endif /* L4_IO_CAN_STORAGE_HPP_ */
