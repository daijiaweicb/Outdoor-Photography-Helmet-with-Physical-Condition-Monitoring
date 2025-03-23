#ifndef BME280_H
#define BME280_H

#include "iic.h"
#include <cstdint>
#include <cmath>
#include <vector>
#include "Timer.h"

#define BME280_ADDRESS 0x76

class BME
{
public:
    struct BMEresults
    {
        float temp;
        float press;
    };

    BMEresults results;

    struct Callbackinterface
    {
        virtual void BMECallback(BMEresults &results) = 0;
        virtual ~Callbackinterface() = default;
    };

    void beginBME();
    void BMERigister(Callbackinterface *ci);

    BME() : iic(1)
    {
    }

    ~BME()
    {
        timer_1s.stop();
        iic.iic_close();
    }

private:
    IIC iic;
    HighPrecisionTimer timer_1s;
    uint16_t dig_T1;
    int16_t dig_T2, dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2, dig_P3, dig_P4, dig_P5;
    int16_t dig_P6, dig_P7, dig_P8, dig_P9;
    int32_t t_fine;

    float compensateTemp(int32_t adc_T);

    float compensatePress(int32_t adc_P);

    void GetData(BMEresults &results);

    void DataReady(BMEresults &results);

    std::vector<Callbackinterface *> BMEcallbackinterface;
};

#endif