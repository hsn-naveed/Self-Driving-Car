#ifndef ICAN_HPP_
#define ICAN_HPP_
#include "can.h"



/**
 * Initializes the CAN bus with FullCAN
 * @param [in] std_list_arr     Pointer to the array of the standard group id list
 * @param [in] std_grp_arr      Pointer to the array of the group id list
 * @returns true if the CAN bus is initialized
 */
bool iCAN_init_FULLCAN(const char * std_list_arr, const char * std_grp_arr);

/**
 * Reads a CAN msg located in RAM using Full CAN
 * @param [in] fc1      Pointer to the can_fullcan_msg_t pointer
 * @param [in] msg_id   Message ID of the CAN msg to retrieve
 * @returns true if the received msg id is the same as the input parameter
 */
bool iCAN_rx(can_fullcan_msg_t *fc1, uint16_t msg_id);

/**
 * Sends a CAN msg so the user only has to fill in the data bytes in the can_msg_t prior to this call
 * @param [in] msg      Pointer to the can_msg_t message
 * @param [in] msg_id   Message id of the CAN msg being sent
 */
bool iCAN_tx(can_msg_t *msg, uint32_t msg_id);

#endif /* 243_CAN_ICAN_HPP_ */
