/*
 * can_definitions.hpp
 *
 *  Created on: Oct 26, 2015
 *      Author: Marvin
 */

#ifndef L4_IO_CAN_DEFINITIONS_HPP_
#define L4_IO_CAN_DEFINITIONS_HPP_

//modes
#define STANDBY_MODE 0x00
#define MANUAL_MODE 0x01
#define AUTO_MODE 0x02
#define FREERUN_MODE 0x03

#define BROADCAST_ID 0x00
#define ANDROID_ID 0x01
#define MASTER_ID 0x02
#define SENSOR_ID 0x03
#define GPS_ID 0x04
#define MOTOR_IO_ID 0x05

//message id's
//Using <source>_<destination> labeling
#define CRITICAL_ERROR_ANDROID 0x008
#define CRTICAL_ERROR_MASTER 0x010
#define CRITICAL_ERROR_SENSOR 0x018
#define CRITICAL_ERROR_GPS 0x020
#define CRITICAL_ERROR_MOTOR_IO 0x028

//#define ANDROID_COMMANDS_MASTER 0x04A
//#define RC_PARAMS 0x08A
//#define SET_DEST 0x0CA
//#define COORDINATES_ANDROID_MASTER 0x10A

//#define MASTER_ANDROID_REG 0x191
//#define COORDINATES_MASTER_ANDROID 0x1D1

//#define MASTER_COMMANDS_SENSOR 0x253
//#define MASTER_COMMANDS_READ_IO 0x295
//#define IO_MOTOR_MASTER_REG 0x2D5
//#define MASTER_COMMANDS_GPS 0x314
//#define GPS_MASTER_REG 0x362
//#define MASTER_COMMANDS_WRITE_IO 0x395
//
//#define HEARTBEAT_ANDROID 0x3CA
//#define HEARTBEAT_SENSOR 0x41A
//#define HEARTBEAT_GPS 0x462
//#define HEARTBEAT_IO_MOTOR 0x4AA
//
//#define HEARBEAT_VALUES 0x4D0


//Sensor
#define SENSOR_MASTER_REG 0x702

//for left and right sensors
#define MINIMUM_SENSOR_VALUE 15

//fot middle and back sensors
#define MINIMUM_SENSOR_BLOCKED_VALUE 10

//Motor
#define MASTER_COMMANDS_MOTOR 0x704

#define COMMAND_MOTOR_FORWARD 0xFF
#define COMMAND_MOTOR_REVERSE 0x55
#define COMMAND_MOTOR_STOP 0x11
 //0xFF = FAST, 0x80 = MEDIUM, 0x11 = SLOW
#define COMMAND_MOTOR_FAST 0xFF
#define COMMAND_MOTOR_MEDIUM 0x80
#define COMMAND_MOTOR_SLOW 0x11


#define COMMAND_MOTOR_LEFT 0x00
#define COMMAND_MOTOR_RIGHT 0xFF
#define COMMAND_MOTOR_STRAIGHT 0x80

#define VALUE_TRUE 0xFF
#define VALUE_FALSE 0x11
#define VALUE_NO_CHANGE 0x00


//Android
#define MANUAL_CONTROL_ENABLED 0xFF
#define MANUAL_CONTROL_DISABLED 0x11

#define SIZE_OF_SENSOR_ARRAY 4
#define READ_SENSOR_DATA 0x01


//Sensor
#define LEFT_SENSOR 0
#define MIDDLE_SENSOR 1
#define RIGHT_SENSOR 2
#define BACK_SENSOR 3

#endif /* L4_IO_CAN_DEFINITIONS_HPP_ */
