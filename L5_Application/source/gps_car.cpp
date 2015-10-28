#include <gps_car.hpp>
#include <stdio.h>
#include "can.h"
#include "io.hpp"



typedef struct {
    uint64_t heading : 8;
} gps_heading_msg_t;

typedef struct {
    uint64_t x_coordinate : 8;
    uint64_t y_coordinate : 8;
} gps_coordinate_msg_t;

void gps_car_init_can_bus(void)
{
    const uint32_t baudrate_kbps = 100;
    const uint16_t queue_size = 16;
    can_std_id_t canid1, canid2;
    canid1 = CAN_gen_sid(can1, 0x314);
    canid2 = CAN_gen_sid(can1, 0x315);

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

bool gps_car_rx(can_fullcan_msg_t *fc1, uint32_t msg_id)
{
    can_fullcan_msg_t fc_temp;
    fc1 = CAN_fullcan_get_entry_ptr(CAN_gen_sid(can1, msg_id));

    return (CAN_fullcan_read_msg_copy(fc1, &fc_temp) && fc1->msg_id == msg_id);
}

bool gps_car_tx(can_msg_t *msg, uint32_t msg_id)
{
    msg->msg_id = msg_id;
    msg->frame = 0;
    msg->msg_id = msg_id;
    msg->frame_fields.is_29bit = 0;
    msg->frame_fields.data_len = 8;

    return CAN_tx(can1, msg, 0);
}

void gps_car_send_heading(void)
{
    can_msg_t *msg;
    uint32_t msg_id = 0x362;

    gps_heading_msg_t *p = (gps_heading_msg_t*) msg->data.bytes[0];
    msg->msg_id = msg_id;

#if 0
    // TODO: Send x and y coordinates in separate function
    msg->data.bytes[1] = x coordinate
    msg->data.bytes[2] = y coordinate
#endif

    p->heading = MS.getHeading();

    if (!gps_car_tx(msg, msg_id)) {
        printf("Failed to send heading\n");
    }
}

void gps_car_send_heartbeat(void)
{
    can_msg_t *msg;
    uint8_t heartbeat_msg = 0x03;
    uint32_t msg_id = 0x462;

    msg->msg_id = msg_id;

#if 0
    // TODO: Send x and y coordinates in separate function
    msg->data.bytes[1] = x coordinate
    msg->data.bytes[2] = y coordinate
#endif

    msg->data.bytes[0] = heartbeat_msg;

    if (!gps_car_tx(msg, msg_id)) {
        printf("Failed to send heartbeat message\n");
    }
}
