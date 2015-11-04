#include <stdlib.h>
#include "iCAN.hpp"
#include "can.h"


bool iCAN_init_FULLCAN(const char * std_list_arr, const char * std_grp_arr, uint16_t FC_entry1, uint16_t FC_entry2)
{
    can_t bus = can1;
    uint32_t baudrate_kbps = 100;
    uint16_t rxq_size = 50;
    uint16_t txq_size = 50;

    size_t num_std_ids = sizeof(std_list_arr);
    size_t num_grp_ids = sizeof(std_grp_arr);
    can_std_id_t slist[num_std_ids];
    can_std_grp_id_t sglist[std_grp_arr/2];

    for (int i = 0; i < num_std_ids; i++) {
        slist[i] = CAN_gen_sid(can1, std_list_arr[i]);
    }

    for (int i = 0; i < num_grp_ids; i=i+2) {
        sglist[i] = { CAN_gen_sid(can1, std_grp_arr[i]), CAN_gen_sid(can1, std_grp_arr[i+1]) };
    }

    if (CAN_init(bus, baudrate_kbps, rxq_size, txq_size, NULL, NULL))
    {
        if(CAN_fullcan_add_entry(bus, CAN_gen_sid(bus, FC_entry1), CAN_gen_sid(bus, FC_entry2))) {
            const can_ext_id_t *elist = NULL;
            const can_ext_grp_id_t *eglist= NULL;

            if (CAN_setup_filter(slist, 4, sglist, 2, elist, 0, eglist, 0)) {
                puts("\nIn Full Can Mode and Filter Setup!");
            }
        }
        else {
            puts("\nFULL CAN FAILED");
        }
        CAN_reset_bus(bus);
        puts("\nInitialization Successful\n");
        return true;
    }
    else {
        puts("\nFailed to Initialize");
        return false;
    }
}

bool iCAN_rx(can_fullcan_msg_t *fc1, uint32_t msg_id)
{
    can_fullcan_msg_t fc_temp;
    fc1 = CAN_fullcan_get_entry_ptr(CAN_gen_sid(can1, msg_id));

    return (CAN_fullcan_read_msg_copy(fc1, &fc_temp) && fc1->msg_id == msg_id);
}

bool iCAN_tx(can_msg_t *msg, uint32_t msg_id)
{
    msg->msg_id = msg_id;
    msg->frame = 0;
    msg->msg_id = msg_id;
    msg->frame_fields.is_29bit = 0;
    msg->frame_fields.data_len = 8;

    return CAN_tx(can1, msg, 0);
}
