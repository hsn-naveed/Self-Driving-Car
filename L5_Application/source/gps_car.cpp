#include <gps_car.hpp>
#include <stdio.h>
#include "can.h"
#include "io.hpp"
#include "243_can/CAN_structs.h"
#include "243_can/iCAN.hpp"



static const uint32_t HEARTBEAT_MSG_SEND_ID = 0x462;
static const uint32_t HEARTBEAT_MSG = 0x03;

void gps_car_init_can_bus(void)
{
    const uint32_t baudrate_kbps = 100;
    const uint16_t queue_size = 16;
    can_std_id_t canid1, canid2;
    canid1 = CAN_gen_sid(can1, 0x314); // XXX: Store these magic numbers in a common "enum" file
    canid2 = CAN_gen_sid(can1, 0x315);

    // Initialize the CAN HW
    CAN_init(can1, baudrate_kbps, queue_size, queue_size, (can_void_func_t) bus_off_cb, 0);

    CAN_reset_bus(can1);

    // XXX: If using FullCAN, then you won't need any RX Queue at all, but TX queue will still be needed to send
    // a bunch of messages at once

    CAN_fullcan_add_entry(can1, canid1, canid2);
    if (2 == CAN_fullcan_get_num_entries()) {
        printf("CAN 1 FullCan entries added\n");
    }
    else {
        // XXX: This should never happen, so just use while(1) loop to stop code execution here
        CAN_bypass_filter_accept_all_msgs();
        printf("CAN 1 failed to add full can entry - setting bypass filter\n");
    }

    // XXX: Recommend a 1Hz  or 10Hz Bus Off callback to reset the CAN Bus
    if (CAN_is_bus_off(can1)) {
        printf("Can bus is off\n");
        CAN_reset_bus(can1);
    }
    else {
        printf("CAN bus is on\n");

    }

    printf("Initialized!\n");
}

// XXX: This is inside an ISR, so shouldn't use printf(), it will likely kill periodic scheduler and reboot
void * bus_off_cb(void)
{
    CAN_reset_bus(can1);
}

void gps_car_send_heading(void)
{
    can_msg_t msg;

    gps_heading_msg_t *p = (gps_heading_msg_t*) &msg.data.bytes[0];
    msg.msg_id = HEADING_MSG_RECV_ID;

#if 0
    // TODO: Send x and y coordinates in separate function
    msg->data.bytes[1] = x coordinate
    msg->data.bytes[2] = y coordinate
#endif

    p->heading = MS.getHeading(); // XXX: this will get casted down to 8-bit uint from float

    if (!iCAN_tx(&msg, HEADING_MSG_RECV_ID)) {
        printf("Failed to send heading\n");
    }
}

void gps_car_send_heartbeat(void)
{
    can_msg_t msg;

    msg.msg_id = HEARTBEAT_MSG_SEND_ID;

#if 0
    // TODO: Send x and y coordinates in separate function
    msg->data.bytes[1] = x coordinate
    msg->data.bytes[2] = y coordinate
#endif

    /// XXX : Put heartbeat in a common typedef struct to be shared by others
    msg.data.bytes[0] = HEARTBEAT_MSG;

    if (!iCAN_tx(&msg, HEARTBEAT_MSG_SEND_ID)) {
        printf("Failed to send heartbeat message\n");
    }
}
