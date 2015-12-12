#include <stdint.h>
#include "io.hpp" // All IO Class definitions
#include "bio.h"
#include "magnetometer_sensor.hpp"
#include <stdio.h>
#include <cmath>



bool Magnetometer_Sensor::init()
{
    writeReg(ConfigA, 0x70); // 8-average, 15Hz default, normal measurement
    writeReg(ConfigB, 0x20); // gain = 5
//    writeReg(Mode, 0x00); // continuous measurments
}

uint8_t Magnetometer_Sensor::getStatus()
{
    return (uint8_t)readReg(Status);
}

// do not use these functions. only use getXYZ(float*);
int16_t Magnetometer_Sensor::getX()
{
    return (int16_t)get16BitRegister(X_MSB);
}

int16_t Magnetometer_Sensor::getY()
{
    return (int16_t)get16BitRegister(Y_MSB);
}

int16_t Magnetometer_Sensor::getZ()
{
    return (int16_t)get16BitRegister(Z_MSB);
}

bool Magnetometer_Sensor::getXYZ(float *xyzBuff)
{
    uint8_t buff[5] = {0};
    int16_t raw[3];
//    getReading(&buff[0], &xyzBuff[0]);
//    uint8_t buffer[256] = { 0 };
    writeReg(0x02, 0x01);
    I2C2::getInstance().writeReg(0x3C, 0x02, 0x01);
    I2C2::getInstance().readRegisters(0x3C, 0x03, buff, 6);

    for (int i = 0; i < 6; i++)
    {
        raw[i/2] = (buff[i] << 8);
        raw[i/2] |= buff[i+1];
        i++;
    }

//    printf("X: %i, Y: %i, Z: %i\n", raw[0], raw[2], raw[1]);
    heading.x = static_cast<float>(raw[0]); //had to use static_cast. (float) broke calculation
    heading.y = static_cast<float>(raw[2]);
    heading.z = static_cast<float>(raw[1]);
//    printf("X: %f, Y: %f, Z: %f\n", heading.x, heading.y, heading.z);
    //normalize values and convert to uT.
    heading.x = heading.x / gauss_XY * 100;
    heading.y = heading.y / gauss_XY * 100;
    heading.z = heading.z / gauss_Z * 100;

    return true;
}

float Magnetometer_Sensor::getHeading()
{
    float buff[3] = {0};
    // buff[0] = X
    // buff[2] = Y
    // buff[1] = Z
    float tempheading = 0, pi = 22/7, xH = 0, yH = 0, declinationAngle = .2268;
    float headingDegrees = 0;

    if (!MS.getXYZ(buff)) {
        printf("Magnetometer sensor read error!");
    }
    else {
//        printf("X: %f, Y: %f, Z: %f\n", heading.x, heading.y, heading.z);
//        xH = (float)buff[0]; // convert to float
//        yH = (float)buff[2]; //convert to float

        tempheading = atan2(heading.y,heading.x);
//        printf("\n\t\t%f", tempheading);
        tempheading += declinationAngle; //compensate for declination
        if (tempheading < 0) {
            tempheading += 2 * pi;
        }
        if (tempheading > (2 * pi)) {
            tempheading -= 2 * pi;
        }
        headingDegrees = (tempheading * 180 / pi);
    }

//    headingDegrees += 35;
    return headingDegrees;
}
