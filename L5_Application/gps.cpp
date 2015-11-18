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

#define dest_y CAN_ST.gps_coords_dest->y_coordinate
#define curr_y CAN_ST.gps_coords_curr->y_coordinate
#define dest_x CAN_ST.gps_coords_dest->x_coordinate
#define curr_x CAN_ST.gps_coords_curr->x_coordinate

uint32_t curr_heading(){
    float degrees = (atan((dest_y-curr_y)/(dest_x - curr_x)) * 180/Pi);

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

    /*
     * Use the distance formula to calculate the distance between current and desired position
     * if the distance is less than Y metres, do STRICT STEERING, else steer with a slight error of X%
     */
    if (curr_heading() == CAN_ST.mAngleValue->heading)//+- X%
            {
        //go straight
    }
    else {}

}
