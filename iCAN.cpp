#include <stdlib.h>
#include "iCAN.hpp"
#include "can.h"
#include "stdio.h"

#if CAN_testing
void canBusError1(){
    CAN_reset_bus(can1);
}
void canBusError2(){
    CAN_reset_bus(can2);
}

bool iCAN_init(const can_std_id_t *slist){


    const can_std_grp_id_t sglist[] = { {CAN_gen_sid(can1, 0x100), CAN_gen_sid(can1, 0x110)}, // Group 1
            {CAN_gen_sid(can2, 0x200), CAN_gen_sid(can2, 0x210)}  // Group 2
    };
    const can_ext_id_t *elist       = NULL; // Not used, so set it to NULL
    const can_ext_grp_id_t eglist[] = {NULL}; // Group 1

    CAN_init(can1, 100, 5, 5, (can_void_func_t)canBusError1, (can_void_func_t)canBusError1); //init can 1
    CAN_init(can2, 100, 5, 5, (can_void_func_t)canBusError2, (can_void_func_t)canBusError2); //init can 2
//        CAN_fullcan_add_entry(can_t::can1, CAN_gen_sid(can_t::can1, 0x100), CAN_gen_sid(can_t::can1, 0x102)); //add filter to can 1
//        CAN_fullcan_add_entry(can_t::can2, CAN_gen_sid(can_t::can2, 0x200), CAN_gen_sid(can_t::can2, 0x202)); //add filter to can 2
//        CAN_fullcan_add_entry(can_t::can2, CAN_gen_sid(can_t::can2, 0x204), CAN_gen_sid(can_t::can2, 0x206)); //add filter to can 2
    for(int i = 0; i < 4; i+=2){
        CAN_fullcan_add_entry(can2, slist[i], slist[i+1]);
    }
    printf("Entries added to can: %i", CAN_fullcan_get_num_entries());
        CAN_setup_filter(slist, 4, sglist, 2, elist, 0, eglist, 0); // setup filter list for both can buses
//    CAN_bypass_filter_accept_all_msgs();
    CAN_reset_bus(can1);
    CAN_reset_bus(can2);
    printf("Can buses initialized\n");
    return true;
}
#endif

bool iCAN_init_FULLCAN(uint16_t * std_list_arr, size_t arraySize)
{
//    can_t bus = can1;
    uint32_t baudrate_kbps = 100;
    uint16_t rxq_size = 50;
    uint16_t txq_size = 50;
    uint16_t blank = 0xFFFF;
    can_std_id_t slist[arraySize] = {0};
    for(int i = 0; i <(int) arraySize; i++){
        printf("std_list_arr[%i]: %x\n", i,  std_list_arr[i]);
        slist[i] = CAN_gen_sid(can1, std_list_arr[i]);
    }
    printf("size of list: %u\n", arraySize);
    if (CAN_init(can1, baudrate_kbps, rxq_size, txq_size, NULL, NULL))
    {
        for (int i = 0; i < (int) arraySize; i+=2) {
            printf("adding entry#: %i", i);
            if(i == ((int) arraySize - 1)){
                CAN_fullcan_add_entry(can1, slist[i], CAN_gen_sid(can1, blank));
            }
            else if(CAN_fullcan_add_entry(can1, slist[i], slist[i+1]));
            else{
                return false;
            }
        }
        if(CAN_fullcan_get_num_entries() == (arraySize + (arraySize%2))){
            CAN_reset_bus(can1);
            puts("\nInitialization Successful\n");
        }
        else{
            puts("\nCould not verify full can entries\n");
            return false;
        }
        return true;
    }
    else {
        puts("\nFailed to Initialize");
        return false;
    }
}

bool iCAN_rx(can_fullcan_msg_t *msg, uint16_t msg_id)
{
    can_fullcan_msg_t fc_temp;
    can_fullcan_msg_t *fc1 = CAN_fullcan_get_entry_ptr(CAN_gen_sid(can1, msg_id));
   // printf("Received the following from can2:\n");
    return (CAN_fullcan_read_msg_copy(fc1, msg) && fc1->msg_id == msg_id);
}

bool iCAN_tx(can_msg_t *msg, uint16_t msg_id)
{
    msg->msg_id = msg_id;
    msg->frame = 0;
    msg->frame_fields.is_29bit = 0;
    msg->frame_fields.data_len = 8;
    return CAN_tx(can1, msg, 0);
}
