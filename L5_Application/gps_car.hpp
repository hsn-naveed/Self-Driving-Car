#ifndef L5_APPLICATION_GPS_CAR_HPP_
#define L5_APPLICATION_GPS_CAR_HPP_
#include "can.h"
#include "GPS.hpp"


/// Initializes the can bus with FullCAN
void gps_car_init_can_bus(void);

/// Callback function if the bus is heavy
void * bus_off_cb(void);

/**
 * Sends the heading direction
 */
void gps_car_send_heading(void);

/**
 * Sends the periodic heartbeat to the Master controller
 */
void gps_car_send_heartbeat(void);

#endif /* L5_APPLICATION_GPS_CAR_HPP_ */
