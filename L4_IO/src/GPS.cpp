/*
 * GPS.cpp
 *
 *  Created on: Oct 15, 2015
 *      Author: Calvin
 */


#include "GPS.hpp"
#include <stdio.h>
#include <string.h>
#include <cstdlib> //atof
#include <cmath>
#include "io.hpp"

GPS_TX_GPS_INFO_HEADING_t headingToTx = {0};
ANDROID_TX_ANDROID_INFO_COORDINATES_t dest[128] = {0};
uint8_t num_rx_checkpoints = 0;
ANDROID_TX_ANDROID_INFO_CHECKPOINTS_t lastDest = {0};

bool GPS_parser::init(void){
    gps_uart.init(9600, 256, 0);
    return true;
}

bool GPS_parser::run(void *p){
    const char *delimiter1 = ",", *delimiter2 = "*";
    char *current = 0;
    char charChecksum[3];
    if(gps_uart.gets(nmeaSentence, NMEA_MAX_LENGTH-1, 100)){
        if(verifyChecksum(charChecksum)){
            const char *GPGGA = strtok_r(nmeaSentence, delimiter1, &current);
            if(strcmp(GPGGA, "$GPGGA") == 0){
                const char *UTC = strtok_r(NULL, delimiter1, &current);
                const char *latitude = strtok_r(NULL, delimiter1, &current);
                const char *nORs = strtok_r(NULL, delimiter1, &current);
                const char *longitude = strtok_r(NULL, delimiter1, &current);
                const char *eORw = strtok_r(NULL, delimiter1, &current);
                const char *garbage = strtok_r(NULL, delimiter2, &current);
                const char *checksum = strtok_r(NULL, delimiter2, &current);
                printf("String type: %s\n", GPGGA);
                printf("UTC: %s\n", UTC);
                printf("Latitude: %s %s\n", latitude, nORs);
                printf("Longitude: %s %s\n", longitude, eORw);
                printf("Checksum: %s\n", checksum);
                if((charChecksum[0] == *checksum) && (charChecksum[1] == *(checksum+1))){
                }
                currentGPS = parseCood(latitude, longitude, nORs, eORw);
            }
            headingToTx.GPS_INFO_HEADING_dst = calculateCorrectHeading();
        }
        else{
            printf("Checksum failed!!!\n");
        }
    }
    else printf("Nothing received from GPS");
    //    printf("%s\n\n", nmeaSentence);
    //    char *nmeaSentence = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
    return true;
}

bool GPS_parser::verifyChecksum(char *charChecksum){
    int j = 1;

    uint16_t calcChecksum = 0;
    bool valid = false;
    if(strstr(nmeaSentence, "$GPGGA")){
        printf("%s\n", nmeaSentence);
        while(nmeaSentence[j] != '*'){
            calcChecksum ^= nmeaSentence[j];
            j++;
        }
        sprintf(charChecksum, "%X", calcChecksum);
        printf("\nchecksum value: %s\n", charChecksum);
        if(charChecksum[0] == nmeaSentence[j+1] && charChecksum[1] == nmeaSentence[j+2]){
            printf("Checksum validated!\n");
            valid = true;
        }
//        else{
//            printf("Checksum incorrect!\n");
//        }
    }
    return valid;
}

ANDROID_TX_ANDROID_INFO_COORDINATES_t GPS_parser::parseCood(const char *latitude, const char *longitude,
                                    const char *nORs, const char *eORw){
    //longitude
    ANDROID_TX_ANDROID_INFO_COORDINATES_t temp;
    char degLat[2] = {0};
    char degLong[3] = {0};
    strncpy(degLat, latitude, 2);
    temp.GPS_INFO_COORDINATES_lat = atof(degLat) + ((atof(latitude) - (atof(degLat)*100)) / 60);
    if(*nORs == 'S'){
        temp.GPS_INFO_COORDINATES_lat *= -1;
    }
    printf("Latitude: %f\n", temp.GPS_INFO_COORDINATES_lat);
    strncpy(degLong, longitude, 3);
    temp.GPS_INFO_COORDINATES_long = atof(degLong) + ((atof(longitude) - (atof(degLong)*100)) / 60);
    if(*eORw == 'W'){
        temp.GPS_INFO_COORDINATES_long *= -1;
    }
    printf("Longitude: %f\n", temp.GPS_INFO_COORDINATES_long);
    return temp;
}

uint32_t GPS_parser::calculateCorrectHeading(){
    const float pi = 3.1415;
    float correctHeading = 0;
    dest[currentDest] = {37.3604355, -122.1282221};
    float currentDist = (currentGPS.GPS_INFO_COORDINATES_lat - dest[currentDest].GPS_INFO_COORDINATES_lat)
                         / (currentGPS.GPS_INFO_COORDINATES_long - dest[currentDest].GPS_INFO_COORDINATES_long);
    printf("Current Dist: %f\n", currentDist);
    if(dest[currentDest].GPS_INFO_COORDINATES_lat == 0 && dest[currentDest].GPS_INFO_COORDINATES_long == 0){
        printf("Destination has been reached or no more destinations in path! \n");
    }
//    else if(currentDist < 5){
//        currentDest++;
//    }
    else{
        float dx = dest[currentDest].GPS_INFO_COORDINATES_lat - currentGPS.GPS_INFO_COORDINATES_lat;
        printf("dx = %f\n", dx);
        float dy = dest[currentDest].GPS_INFO_COORDINATES_long - currentGPS.GPS_INFO_COORDINATES_long;
        printf("dy = %f\n", dy);
        correctHeading = atan2(dy, dx) * 180 / pi;
        if(correctHeading < 0){
            correctHeading += 360;
        }
        else if(correctHeading > 360){
            correctHeading -= 360;
        }
    }
    printf("The correct heading is: %i\n", (uint32_t) correctHeading);
    return (uint32_t) correctHeading;
}
