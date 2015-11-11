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


bool GPS_parser::init(void){
    gps_uart.init(9600, 256, 0);
//    QueueHandle_t myQueue = xQueueCreate(1, sizeof());
    return true;
}

bool GPS_parser::run(void *p){
    const char *delimiter1 = ",", *delimiter2 = "*";
    char *current = 0;
    int j = 1;
    char charChecksum[3];
    uint16_t calcChecksum = 0;
    if(gps_uart.gets(nmeaSentence, NMEA_MAX_LENGTH-1, 100)){
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
            }
            else{
                printf("Checksum incorrect!\n");
            }
        }
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
    }
    else printf("Nothing received from GPS");
//    printf("%s\n\n", nmeaSentence);
//    char *nmeaSentence = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
    return true;
}

bool GPS_parser::verifyChecksum(){
    bool valid = false;
    return valid;
}

gps_data_t GPS_parser::parseCood(const char *latitude, const char *longitude,
                                    const char *nORs, const char *eORw){
    //longitude
    gps_data_t temp;
    char degLat[2] = {0};
    char degLong[3] = {0};
    strncpy(degLat, latitude, 2);
    temp.latitude = atof(degLat) + ((atof(latitude) - (atof(degLat)*100)) / 60);
    if(*nORs == 'S'){
        temp.latitude *= -1;
    }
    printf("Latitude: %f\n", temp.latitude);
    strncpy(degLong, longitude, 3);
    temp.longitude = atof(degLong) + ((atof(longitude) - (atof(degLong)*100)) / 60);
    if(*eORw == 'W'){
        temp.longitude *= -1;
    }
    printf("Longitude: %f", temp.longitude);
    return temp;
}
