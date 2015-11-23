/*
 * sensor.h
 *
 *  Created on: Nov 6, 2015
 *      Author: Administrator
 */

#ifndef L5_APPLICATION_SENSOR_HPP_
#define L5_APPLICATION_SENSOR_HPP_

#include <stdio.h>
#include "io.hpp"
#include "lpc_sys.h"
#include "gpio.hpp"
#include "243_can/CAN_structs.h"
#include "utilities.h" //delay lib
#include <L3_Utils/singleton_template.hpp>
#include <243_can/CAN_structs.h>
#include "can.h"
#include "semphr.h"
#include "eint.h"
#include "243_can/iCAN.hpp"


extern GPIO  Left_en;   // left RXpin USE EXTERN so they wont be redeclared every time "sensor.hpp" is called!
extern GPIO Middle_en; // Middle
extern GPIO Right_en; // Right
extern GPIO Rear_en;  //Rear

//extern can_msg_t *sonarMsg  ;


     void calc_dist_left(void);
     void calc_dist_middle(void);
     void calc_dist_right(void);
     void calc_dist_rear(void);

     void Range_left(void);
     void Range_middle(void);
     void Range_right(void);
     void Range_rear(void);

     void Range(void);

     void test(void);

     void CAN_send(void);

     void store (int in);
     void filter(int in, int out);




#endif /* L5_APPLICATION_SENSOR_HPP_ */
