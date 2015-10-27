#ifndef L4_IO_MAGNETOMETER_SENSOR_HPP_
#define L4_IO_MAGNETOMETER_SENSOR_HPP_

#include <stdint.h>
#include "i2c2_device.hpp"  // I2C Device Base Class



struct rawHeading{
        float x = 0;
        float y = 0;
        float z = 0;
};

class Magnetometer_Sensor : private i2c2_device, public SingletonTemplate<Magnetometer_Sensor>{
    public:
        bool init();

        uint8_t  getStatus();
        int16_t getX();
        int16_t getY();
        int16_t getZ();
        bool getXYZ(float *xyzBuff);
        float getHeading();

    private:
        rawHeading heading;
        int gauss_XY = 1100;
        int gauss_Z = 980;
        Magnetometer_Sensor() : i2c2_device(I2CAddr_Magnetometer)
        {
        }
        friend class SingletonTemplate<Magnetometer_Sensor>;

        static const unsigned char mWhoAmIExpectedValue = 0x48;
        typedef enum{
            Status=9,
            X_MSB=3, X_LSB=4,
            Y_MSB=7, Y_LSB=8,
            Z_MSB=5, Z_LSB=6,

            WhoAmI=10,
            ConfigA=0, ConfigB=1,
            Mode=2
        } __attribute__ ((packed)) RegisterMap;
};


#endif /* L4_IO_MAGNETOMETER_SENSOR_HPP_ */
