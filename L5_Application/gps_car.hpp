#ifndef L5_APPLICATION_GPS_CAR_HPP_
#define L5_APPLICATION_GPS_CAR_HPP_
#include "can.h"



/// Initializes the can bus with FullCAN
void gps_car_init_can_bus(void);

/// Callback function if the bus is heavy
void * bus_off_cb(void);

/**
 * Reads a CAN msg located in RAM using Full CAN
 * @param [in] fc1      Pointer to the can_fullcan_msg_t pointer
 * @param [in] msg_id   Message ID of the CAN msg to retrieve
 * @returns true if the received msg id is the same as the input parameter
 */
bool gps_car_rx(can_fullcan_msg_t *fc1, uint16_t msg_id);

/**
 * Sends a CAN msg so the user only has to fill in the data bytes
 * @param [in] msg      Pointer to the can_msg_t message
 * @param [in] msg_id   Message id of the CAN msg being sent
 */
bool gps_car_tx(can_msg_t *msg, uint32_t msg_id);

/**
 * Sends the heading direction
 */
void gps_car_send_heading(void);

/**
 * Sends the periodic heartbeat to the Master controller
 */
void gps_car_send_heartbeat(void);

#endif /* L5_APPLICATION_GPS_CAR_HPP_ */
