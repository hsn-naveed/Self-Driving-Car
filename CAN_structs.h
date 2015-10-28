#ifndef L5_APPLICATION_CAN_STRUCTS_H_
#define L5_APPLICATION_CAN_STRUCTS_H_



// MSG ID: 0x362
typedef struct {
    uint64_t heading : 8;
} gps_heading_msg_t;

// MSG ID: 0x###
typedef struct {
    uint64_t x_coordinate : 8;
    uint64_t y_coordinate : 8;
} gps_coordinate_msg_t;

#endif /* L5_APPLICATION_CAN_STRUCTS_H_ */
