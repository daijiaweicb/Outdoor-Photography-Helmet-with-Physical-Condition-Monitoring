#ifndef BME280_H
#define BME280_H

#include "iic.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cstdint>
#include <cmath>

#define BMP280_ADDRESS 0x76
#define BMP280_ID 0x58

class BMP{
private:


    uint16_t dig_T1;
    int16_t dig_T2, dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2, dig_P3, dig_P4, dig_P5;
    int16_t dig_P6, dig_P7, dig_P8, dig_P9;

    int32_t t_fine;


    void beginBMP();

    


public:
    IIC iic;

    





};





#endif