/*
 * sensor.hpp
 *
 *  Created on: Nov 2, 2015
 *      Author: Administrator
 */

#ifndef SENSOR_HPP_
#define SENSOR_HPP_

#include <stdio.h>
#include"io.hpp"
#include "lpc_sys.h"


#include"gpio.hpp"
#include "243_can/CAN_structs.h"
//#include"iCAN.hpp"
#include"utilities.h" //delay lib

#include <L3_Utils/singleton_template.hpp>
#include <243_can/CAN_structs.h>
#include "can.h"


class sensor : public SingletonTemplate <sensor>
{
public:
        static int Left_trig_time,Middle_trig_time,Right_trig_time,Rear_trig_time;

        static int left_dist,
            middle_dist,
            right_dist,
            rear_dist;

        can_msg_t *sonarMsg ;


        //interrupt enabled GPIO ports Falling Edge

        static void calc_dist_left(void);
        static void calc_dist_middle(void);
        static void calc_dist_right(void);
        static void calc_dist_rear(void);

        void Range_left(void);
        void Range_middle(void);
        void Range_right(void);
        void Range_rear(void);
        void CAN_send(void);
        bool init(void);

        //void print(void);

private:
        sensor() {

        }
     //   ~sensor();

        friend class SingletonTemplate<sensor>;  ///< Friend class used for Singleton Template

};


//Sonar enable pins init , pull up for >20uS to start ranging
GPIO Left_en(P2_7);   // left
GPIO Middle_en(P2_6); // Middle
GPIO Right_en(P0_30); // Right
GPIO Rear_en(P0_29);  // Rear


#endif /* SENSOR_HPP_ */
