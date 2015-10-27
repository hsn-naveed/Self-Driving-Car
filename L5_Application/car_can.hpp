#ifndef L5_APPLICATION_CAR_CAN_HPP_
#define L5_APPLICATION_CAR_CAN_HPP_
#include "can.h"



/// Initializes the can bus with FullCAN
void car_can_init_can_bus(void);

/// Callback function if the bus is heavy
void * bus_off_cb(void);

/**
 * Reads a CAN msg located in RAM using Full CAN
 * @param [in] fc1      Pointer to the can_fullcan_msg_t pointer
 * @param [in] msg_id   Message ID of the CAN msg to retrieve
 * @returns true if the received msg id is the same as the input parameter
 */
bool car_can_rx(can_fullcan_msg_t *fc1, uint16_t msg_id);

/**
 * Sends a CAN msg so the user only has to fill in the data bytes
 * @param [in] msg      Pointer to the can_msg_t message
 * @param [in] msg_id   Message id of the CAN msg being sent
 */
bool car_can_tx(can_msg_t *msg, uint32_t msg_id);

/**
 * Sends the heading direction
 * @returns true if msg sent successfully
 */
void car_can_send_heading(void);

#endif /* L5_APPLICATION_CAR_CAN_HPP_ */
