#include <stdio.h>
#include "car_can.hpp"
#include "can.h"
#include "io.hpp"


void car_can_init_can_bus(void)
{
    const uint32_t baudrate_kbps = 100;
    const uint16_t queue_size = 16;
    can_std_id_t canid1, canid2;
    canid1 = CAN_gen_sid(can1, 0x101);
    canid2 = CAN_gen_sid(can1, 0x102);

    // Initialize the magnetometer sensors
    MS.init();

    // Initialize the CAN HW
    CAN_init(can1, baudrate_kbps, queue_size, queue_size, (can_void_func_t) bus_off_cb, 0);

    CAN_reset_bus(can1);

    CAN_fullcan_add_entry(can1, canid1, canid2);
    if (2 == CAN_fullcan_get_num_entries()) {
        printf("CAN 1 FullCan entries added\n");
    }
    else {
        CAN_bypass_filter_accept_all_msgs();
        printf("CAN 1 failed to add full can entry - setting bypass filter\n");
    }

    if (CAN_is_bus_off(can1)) {
        printf("Can bus is off\n");
        CAN_reset_bus(can1);
    }
    else {
        printf("CAN bus is on\n");

    }

    printf("Initialized!\n");
}

void * bus_off_cb(void)
{
    printf("Bus detection =  off ~ resetting CAN bus!\n");
    CAN_reset_bus(can1);
}

bool car_can_rx(can_fullcan_msg_t *fc1, uint32_t msg_id)
{
    can_fullcan_msg_t fc_temp;

    fc1 = CAN_fullcan_get_entry_ptr(CAN_gen_sid(can1, msg_id));

    CAN_fullcan_read_msg_copy(fc1, &fc_temp);

    return (msg_id == fc1->msg_id);
}

bool car_can_tx(can_msg_t *msg, uint32_t msg_id)
{
    msg->msg_id = msg_id;
    msg->frame = 0;
    msg->msg_id = msg_id;
    msg->frame_fields.is_29bit = 0;
    msg->frame_fields.data_len = 8;

    return CAN_tx(can1, msg, 0);
}

void car_can_send_heading(void)
{
    can_msg_t *msg;

    uint32_t msg_id = 0x362;
    msg->msg_id = msg_id;

#if 0
    // TODO: Send x and y coordinates in separate function
    msg->data.bytes[1] = x coordinate
    msg->data.bytes[2] = y coordinate
#endif
    msg->data.bytes[0] = MS.getHeading();

    if (!car_can_tx(msg, msg_id)) {
        printf("Failed to send Coordinates and heading\n");
    }
}
