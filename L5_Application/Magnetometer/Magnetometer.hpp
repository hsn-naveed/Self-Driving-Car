/*
 * Magnetometer.hpp
 *
 *  Created on: Apr 27, 2015
 *      Author: calvin.lai
 */

#ifndef L5_APPLICATION_MAGNETOMETER_MAGNETOMETER_HPP_
#define L5_APPLICATION_MAGNETOMETER_MAGNETOMETER_HPP_

class Magnetometer : public scheduler_task{
    public:
        Magnetometer(uint8_t priority);
        bool run(void *p);
};


#endif /* L5_APPLICATION_MAGNETOMETER_MAGNETOMETER_HPP_ */
