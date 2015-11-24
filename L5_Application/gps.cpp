/*
 * gps.cpp
 *
 *  Created on: Nov 9, 2015
 *      Author: Hassan
 */

//#include "243_can/CAN_structs.h"
#include "can_storage.hpp"
#include "io.hpp"
#include "math.h"
#include <stdlib.h>
//#include "periodic_scheduler/period_callbacks.cpp"
//#include "periodic_scheduler/periodic_callback.h"


#define Pi 3.14159265

uint32_t des_heading(){
    float degrees = atan((CAN_ST.gps_coords_dest->y_coordinate - CAN_ST.gps_coords_curr->y_coordinate)/(CAN_ST.gps_coords_dest->x_coordinate - CAN_ST.gps_coords_curr->x_coordinate)) * 180/Pi;


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


/* If the destination and current heading are within Tolerance degrees of each other, steer straight
 * Else steer left or right depending upon:
 * 1. Mutual angle
 * 2. Which of the two values is bigger
 */

void setDirection(){
    int tolerance = 20;
    if ((CAN_ST.mAngleValue->heading >= (des_heading()-tolerance)) and (CAN_ST.mAngleValue->heading <= (des_heading()+tolerance)))//+- X%
        {
        //go straight
        //generateMotorCommands(COMMAND_MOTOR_FORWARD_STRAIGHT);
    }
    else if(des_heading()>CAN_ST.mAngleValue->heading) {
        if (abs(des_heading()-CAN_ST.mAngleValue->heading)>180){
            //generateMotorCommands(COMMAND_MOTOR_FORWARD_RIGHT);
        }
        else{
            //generateMotorCommands(COMMAND_MOTOR_FORWARD_LEFT);
        }
    }
    else{
        if (abs(des_heading()-CAN_ST.mAngleValue->heading)>180){
            //generateMotorCommands(COMMAND_MOTOR_FORWARD_LEFT);
        }
        else{
            //generateMotorCommands(COMMAND_MOTOR_FORWARD_RIGHT);
        }
        if (abs(des_heading()-CAN_ST.mAngleValue->heading)>180){
            //generateMotorCommands(COMMAND_MOTOR_FORWARD_RIGHT);
        }
        else{
            //generateMotorCommands(COMMAND_MOTOR_FORWARD_LEFT);
        }
    }
}
