/*
 * GPS.hpp
 *
 *  Created on: Oct 15, 2015
 *      Author: Calvin
 */

#include "Uart3.hpp"
#include "scheduler_task.hpp"
#include "243_can\iCAN.hpp"
#include "globalVars.h"

#ifndef L4_IO_GPS_HPP_
#define L4_IO_GPS_HPP_

#define NMEA_MAX_LENGTH 200

class GPS_parser : public scheduler_task{
    public:
        GPS_parser(uint8_t priority) : scheduler_task("GPS_parser", 2048, priority),
            gps_uart(Uart3::getInstance()){
            gps_uart.init(9600, gps_uart_rx_size, 1);
            setRunDuration(250);
        };
        bool init(void);
        bool run(void *p);
        bool verifyChecksum();
        ANDROID_TX_ANDROID_INFO_COORDINATES_t parseCood(const char *longitude, const char *latitude,
                                const char *nORs, const char *eORw);
        uint32_t calculateCorrectHeading();
    private:
        Uart3 &gps_uart;
        ANDROID_TX_ANDROID_INFO_COORDINATES_t currentGPS;
        int gps_uart_rx_size = NMEA_MAX_LENGTH;
        char nmeaSentence[NMEA_MAX_LENGTH];
};


#endif /* L4_IO_GPS_HPP_ */
