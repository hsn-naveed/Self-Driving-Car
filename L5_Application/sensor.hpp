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
#include"utilities.h" //delay lib
#include <L3_Utils/singleton_template.hpp>
#include <243_can/CAN_structs.h>
#include "can.h"
#include "semphr.h"


class sensor : public SingletonTemplate <sensor>
{
public:
        //define the cariables here as static then in sensor.cpp
        static int Left_trig_time,Middle_trig_time,Right_trig_time,Rear_trig_time;

        static int left_dist,
            middle_dist,
            right_dist,
            rear_dist;

        can_msg_t *sonarMsg ;

        SemaphoreHandle_t left_sem   = xSemaphoreCreateMutex();   //**can't use the same semaphor for more than 2 tasks!
        SemaphoreHandle_t middle_sem = xSemaphoreCreateMutex();   //give to middle
        SemaphoreHandle_t right_sem  = xSemaphoreCreateMutex();   //give to right
        SemaphoreHandle_t rear_sem   = xSemaphoreCreateMutex();   //give it to rear



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
        sensor();
        ~sensor();
//declare the GPIO here, define the port values in the sensor .cpp
        static GPIO Left_en;   // left
        static GPIO Middle_en; // Middle
        static GPIO Right_en; // Right
        static GPIO Rear_en;

        friend class SingletonTemplate<sensor>;  ///< Friend class used for Singleton Template

};


//Sonar enable pins init , pull up for >20uS to start ranging


#endif /* SENSOR_HPP_ */
