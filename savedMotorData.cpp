/*
 * savedMotorData.cpp
 *
 *  Created on: Nov 2, 2015
 *      Author: Jashan
 */

#include <stdint.h>

typedef struct{
        uint8_t motor_direction;
        uint8_t servo_direction;
        uint8_t motor_speed;

}CAN_motor_data_struct;

