#ifndef ICAN_HPP_
#define ICAN_HPP_
#include "can.h"

#define CAN_testing 0

#if CAN_testing
/**
 * test init function
 * initializes with a default acceptance list
 */
bool iCAN_init(const can_std_id_t *slist);
#endif
/**
 * Initializes the CAN bus with FullCAN
 * Assumes you are using can1
 * @param [in] std_list_arr     Pointer to the array of the standard group id list
 *                              array should be of type uint16_t
 * @param [in] arraySize        Size of array. Use (sizeof(std_list_arr) / sizeof(*std_list_arr))
 * @returns true if the CAN bus is initialized
 * @code
 *     uint16_t accepted_msg_ids[] = {0x100, 0x102, 0x104, 0x200};
 *     iCAN_init_FULLCAN(accepted_msg_ids, sizeof(accepted_msg_ids) / sizeof(*accepted_msg_ids);
 * @endcode
 */
bool iCAN_init_FULLCAN(uint16_t * std_list_arr, size_t arraySize);

/**
 * Reads a CAN msg located in RAM using Full CAN to *msg
 * @param [in] msg      Pointer to the can_fullcan_msg_t pointer
 * @param [in] msg_id   Message ID of the CAN msg to retrieve
 * @returns true if the received msg id is the same as the input parameter
 * @code
 *     can_fullcan_msg_t fc_msg;
 *     iCAN_rx(&fc_msg, msg_id_to_check);
 * @endcode
 */
bool iCAN_rx(can_fullcan_msg_t *msg, uint16_t msg_id);

/**
 * Sends a CAN msg so the user only has to fill in the data bytes in the can_msg_t prior to this call
 * @param [in] msg      Pointer to the can_msg_t message
 * @param [in] msg_id   Message id of the CAN msg being sent
 */
bool iCAN_tx(can_msg_t *msg, uint16_t msg_id);

#endif /* 243_CAN_ICAN_HPP_ */
