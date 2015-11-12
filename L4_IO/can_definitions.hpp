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



#endif /* L4_IO_CAN_DEFINITIONS_HPP_ */
