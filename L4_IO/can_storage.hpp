/*
 * can_storage.hpp
 *
 *  Created on: Oct 26, 2015
 *      Author: Marvin
 */

#ifndef L4_IO_CAN_STORAGE_HPP_
#define L4_IO_CAN_STORAGE_HPP_

#include <can_definitions.hpp>
#include <stdint.h>
#include <L3_Utils/singleton_template.hpp>


/**
 * LED class used to control the Board's 8 output LEDs
 *
 * @ingroup BoardIO
 */

class CAN_STORAGE : public SingletonTemplate <CAN_STORAGE>
{
    public:
        bool init(); ///< Initializes this device, @returns true if successful

        //Android
        uint8_t getManualControlValue() {return mManualControlValue;}
        void setManualControlValue(uint8_t val) {mManualControlValue = val;}

        uint8_t getGoSignalValue() {return mGoSignal;}
        void setGoSignalValue(uint8_t val) { mGoSignal = val;}

        //Motor
        int getMotorSpeed() {return mMotorSpeed;}
        void setMotorSpeed(uint8_t speed) { mMotorSpeed = speed;}

        //Sensor
        uint8_t* getSensorValues() {return mSensorValues;}
        void setSensorValues(uint8_t arr[], int size);


    private:


        //Android


               //byte[0] of ANDROID_COMMANDS -> MAS
               //ENABLE: 0xFF
               //DISABLE: 0x11
               uint8_t mManualControlValue;
               uint8_t mGoSignal;

               //GPS
               uint32_t mCheckPointX[8]; //contains X coordinates of the checkpoints. 8 max check points supported
               uint32_t mCheckPointY[8]; //contains Y coordinates of the checkpoints

               //if there's only one checkpoint, mCheckPoint[0] will be equal to mFinalDestination
               uint32_t mFinalDestinationX; //contains X coordinates of the final destination
               uint32_t mFinalDestinationY; //contains Y coordinates of the final destination
               uint16_t mAngleValue; //angle value with respect to the current checkpoint

               //Sensor
               uint8_t mSensorValues[(int)SIZE_OF_SENSOR_ARRAY]; //current sensor values; 4 total sensors

        //Motor
        int mMotorSpeed;
        int mMotorDirection;
        int mMotorTurn;

        ///< Private constructor of this Singleton class
          /*  CAN_STORAGE() :  mMotorSpeed(0)  {

                for (int i = 0; i < (int)SIZE_OF_SENSOR_ARRAY; i++){
                       mSensorValue[i] = 0;
                       //global_sensor_value[i] = mSensorValue[i];
                   }

            }*/
        CAN_STORAGE();
        friend class SingletonTemplate<CAN_STORAGE>;  ///< Friend class used for Singleton Template
};

#endif /* L4_IO_CAN_STORAGE_HPP_ */
