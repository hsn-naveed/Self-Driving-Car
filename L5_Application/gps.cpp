/*
 * gps.cpp
 *
 *  Created on: Nov 9, 2015
 *      Author: Hassan
 */

#include "243_can/CAN_structs.h"
#include "can_storage.hpp"
#include "io.hpp"
#include "math.h"

#define Pi 3.14159265
uint64_t curr_heading(){
    return(atan((CAN_ST.gps_coords_dest->y_coordinate-CAN_ST.gps_coords_curr->y_coordinate)/(CAN_ST.gps_coords_dest->x_coordinate-CAN_ST.gps_coords_curr->x_coordinate)) * 180/Pi);
}


