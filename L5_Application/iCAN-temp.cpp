#include <iCAN-temp.hpp>
#include <stdlib.h>
#include "can.h"



//bool iCAN_rx(can_fullcan_msg_t *fc1, uint32_t msg_id)
//{
//    can_fullcan_msg_t fc_temp;
//    fc1 = CAN_fullcan_get_entry_ptr(CAN_gen_sid(can1, msg_id));
//
//    return (CAN_fullcan_read_msg_copy(fc1, &fc_temp) && fc1->msg_id == msg_id);
//}
//
//bool iCAN_tx(can_msg_t *msg, uint32_t msg_id)
//{
//    msg->msg_id = msg_id;
//    msg->frame = 0;
//    msg->msg_id = msg_id;
//    msg->frame_fields.is_29bit = 0;
//    msg->frame_fields.data_len = 8;
//
//    return CAN_tx(can1, msg, 0);
//}
