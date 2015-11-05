#ifndef L5_APPLICATION_CAN_STRUCTS_H_
#define L5_APPLICATION_CAN_STRUCTS_H_

#include <stdint.h>
#include <stdbool.h>

/*
 * MESSAGE IDS          SOURCE          [   7   ][  6   ][  5   ][  4   ][  3   ][  2   ][  1   ][  0   ]
 *
 *
 *                                      //to be parsed by Master
 * 0x702                Sensor          [   7   ][  6   ]   5   ][  4   ][ back ][right ][ mid  ][ left ]
 *
 *                                      //Commands Motor
 * 0x704                Master          [   7   ][  6   ][  5   ][  4   ][  3   ][  SPD ][  L/R ][F/R/S ]
 *
 *                                      //to be parsed by Master and Android
 * 0x706                GPS             [               x-coor          ][          y-coor              ]
 *
 *                                      //to be parsed by Master
 * 0x708                GPS             [   7   ][  6   ][  5   ][  4   ][          heading             ]
 *
 *                                      //Set number of Checkpoints to Master
 * 0x70A                Android         [   7   ][  6   ][  5   ][  4   ][  3   ][  2   ][  1   ][   N  ]
 *
 *                                      // N-CheckPoints
 * 0x70C                Android         [               x-coor          ][          y-coor              ]
 *
 *                                      // Stop/Go Signal to Master
 * 0x70E                Android         [   7   ][  6   ][  5   ][  4   ][  3   ][  2   ][  1   ][ S/Go ]
 *
 *                                      //HearBeat
 * 0x712                Master_H        [   7   ][  6   ][  5   ][  4   ][  3   ][  2   ][  1   ][ 0x00 ]
 *
 * 0x714                Android_H       [   7   ][  6   ][  5   ][  4   ][  3   ][  2   ][  1   ][ 0x00 ]
 *
 * 0x716                Sensor_H        [   7   ][  6   ][  5   ][  4   ][  3   ][  2   ][  1   ][ 0x00 ]
 *
 * 0x718                GPS_H           [   7   ][  6   ][  5   ][  4   ][  3   ][  2   ][  1   ][ 0x00 ]
 *
 * 0x71A                MotLCD_H        [   7   ][  6   ][  5   ][  4   ][  3   ][  2   ][  1   ][ 0x00 ]
 *
 */

//sensor values
typedef struct  {
        uint64_t L : 8;
        uint64_t M : 8;
        uint64_t R : 8;
        uint64_t B : 8;

} __attribute__((__packed__)) sen_msg_t ;


typedef struct  {
        uint64_t FRS : 8; //0xFF = Forward, 0x55 = Reverse, 0x11 = Stop
        uint64_t LR : 8; //range 0x00 to 0xFF ; LEFT to RIGHT, 0x80 = straight
        uint64_t SPD : 8; //0xFF = FAST, 0x55 = MEDIUM, 0x11 = SLOW

} __attribute__((__packed__)) mast_mot_msg_t ;


const uint32_t HEADING_MSG_RECV_ID = 0x362;
typedef struct {
    uint64_t heading : 8;
} gps_heading_msg_t;

// MSG ID: 0x###
typedef struct {
    uint64_t x_coordinate : 32;
    uint64_t y_coordinate : 32;
} gps_coordinate_msg_t;

#endif /* L5_APPLICATION_CAN_STRUCTS_H_ */
