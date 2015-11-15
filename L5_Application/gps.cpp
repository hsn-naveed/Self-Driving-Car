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

uint32_t curr_heading(){
    float degrees = (atan((CAN_ST.gps_coords_dest->y_coordinate-CAN_ST.gps_coords_curr->y_coordinate)/(CAN_ST.gps_coords_dest->x_coordinate-CAN_ST.gps_coords_curr->x_coordinate)) * 180/Pi);

    //First quadrant (dest.y-curr.y)>0 and (dest.x-curr.x)>0
    if (((CAN_ST.gps_coords_dest->y_coordinate-CAN_ST.gps_coords_curr->y_coordinate)>0) && ((CAN_ST.gps_coords_dest->x_coordinate-CAN_ST.gps_coords_curr->x_coordinate)>0)){
        return degrees;
    }
    //Second quadrant (dest.y-curr.y)>0 and (dest.x-curr.x)<0
    else if(((CAN_ST.gps_coords_dest->y_coordinate-CAN_ST.gps_coords_curr->y_coordinate))>0 && ((CAN_ST.gps_coords_dest->x_coordinate-CAN_ST.gps_coords_curr->x_coordinate)<0)){
        return (180 - degrees );
    }
    //Third quadrant (dest.y-curr.y)<0 and (dest.x-curr.x)<0
    else if(((CAN_ST.gps_coords_dest->y_coordinate-CAN_ST.gps_coords_curr->y_coordinate)<0) && ((CAN_ST.gps_coords_dest->x_coordinate-CAN_ST.gps_coords_curr->x_coordinate)<0)){
            return (180 + degrees );
        }
   //Fourth Quadrant
    else {
                return (360 - degrees );
    }
}

uint16_t setDirection(){
    if (curr_heading() == (uint32_t) CAN_ST.mAngleValue){

    }

    return 0; //CHANGE THIS
}
